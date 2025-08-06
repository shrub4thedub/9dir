#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <pthread.h>

#include "icons.h"
#include "theme.h"
#include "dialogs.h"
#include "config.h"

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

typedef struct Point Point;
typedef struct Rectangle Rectangle;
typedef struct DirEntry DirEntry;

struct Point {
    int x, y;
};

struct Rectangle {
    Point min, max;
};

struct DirEntry {
    char *name;
    off_t size;
    time_t mtime;
    mode_t mode;
};

enum {
    Toolpadding = 4,
    Padding = 1,
    Scrollwidth = 14,
    Slowscroll = 10,
};

enum {
    Mrename,
    Mdelete,
};

// Global variables
Display *display;
Window window;
GC gc;
int screen_num;
XFontStruct *font;
Colormap colormap;
Color toolbg, toolfg, viewbg, viewfg, selbg, selfg, scrollbg, scrollfg, high;
Rectangle toolr, homer, upr, cdr, newdirr, newfiler, viewr, scrollr, scrposr, pathr;

char *home;
char path[PATH_MAX];
DirEntry *dirs = NULL;
int ndirs = 0;
int offset = 0;
int nlines = 0;
int lineh = 0;
int sizew = 0;
int scrolling = 0;
int oldbuttons = 0;
int lastn = -1;
int width = 800, height = 600;
Config *config;

// X11 utility functions
Point pt(int x, int y) {
    return (Point){x, y};
}

Rectangle rect(int x1, int y1, int x2, int y2) {
    return (Rectangle){{x1, y1}, {x2, y2}};
}

Point addpt(Point a, Point b) {
    return pt(a.x + b.x, a.y + b.y);
}

int dx(Rectangle r) {
    return r.max.x - r.min.x;
}

int dy(Rectangle r) {
    return r.max.y - r.min.y;
}

int ptinrect(Point p, Rectangle r) {
    return p.x >= r.min.x && p.x < r.max.x && p.y >= r.min.y && p.y < r.max.y;
}

// Color allocation
Color alloccolor(unsigned long rgb) {
    Color c;
    c.xcolor.red = ((rgb >> 16) & 0xFF) << 8;
    c.xcolor.green = ((rgb >> 8) & 0xFF) << 8;
    c.xcolor.blue = (rgb & 0xFF) << 8;
    c.xcolor.flags = DoRed | DoGreen | DoBlue;
    
    if (XAllocColor(display, colormap, &c.xcolor)) {
        c.pixel = c.xcolor.pixel;
    } else {
        c.pixel = BlackPixel(display, screen_num);
    }
    return c;
}

void initcolors(void) {
    // Use config-based colors
    toolbg = parse_color_config(display, colormap, config->toolbar_bg);
    toolfg = parse_color_config(display, colormap, config->toolbar_fg);
    viewbg = parse_color_config(display, colormap, config->bg_color);
    viewfg = parse_color_config(display, colormap, config->fg_color);
    selbg = parse_color_config(display, colormap, config->selected_bg);
    selfg = parse_color_config(display, colormap, config->selected_fg);
    scrollbg = parse_color_config(display, colormap, config->scrollbar_bg);
    scrollfg = parse_color_config(display, colormap, config->scrollbar_fg);
    high = parse_color_config(display, colormap, config->highlight_color);
}

// File operations
void readhome(void) {
    home = getenv("HOME");
    if (home == NULL) {
        home = "/";
    }
}

char *abspath(char *wd, char *p) {
    static char result[PATH_MAX];
    if (p[0] == '/') {
        strcpy(result, p);
    } else {
        snprintf(result, sizeof(result), "%s/%s", wd, p);
    }
    return result;
}

int dircmp(const void *a, const void *b) {
    const DirEntry *da = (const DirEntry *)a;
    const DirEntry *db = (const DirEntry *)b;
    
    int adir = S_ISDIR(da->mode);
    int bdir = S_ISDIR(db->mode);
    
    if (adir != bdir) {
        return bdir - adir; // directories first
    }
    return strcmp(da->name, db->name);
}

void loaddirs(void) {
    DIR *d;
    struct dirent *ent;
    struct stat st;
    char fullpath[PATH_MAX];
    int count = 0;
    off_t maxsize = 1;
    
    // Free existing entries
    if (dirs) {
        for (int i = 0; i < ndirs; i++) {
            free(dirs[i].name);
        }
        free(dirs);
        dirs = NULL;
    }
    
    d = opendir(path);
    if (d == NULL) {
        char errmsg[512];
        snprintf(errmsg, sizeof(errmsg), "Cannot open directory %s: %s", path, strerror(errno));
        alert("Directory Error", errmsg);
        ndirs = 0;
        return;
    }
    
    // Count entries first
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0) continue;
        
        // Skip hidden files if not configured to show them
        if (!config->show_hidden_files && ent->d_name[0] == '.' && strcmp(ent->d_name, "..") != 0) continue;
        
        count++;
    }
    rewinddir(d);
    
    dirs = malloc(count * sizeof(DirEntry));
    if (dirs == NULL) {
        closedir(d);
        ndirs = 0;
        return;
    }
    
    ndirs = 0;
    while ((ent = readdir(d)) != NULL && ndirs < count) {
        if (strcmp(ent->d_name, ".") == 0) continue;
        
        // Skip hidden files if not configured to show them
        if (!config->show_hidden_files && ent->d_name[0] == '.' && strcmp(ent->d_name, "..") != 0) continue;
        
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, ent->d_name);
        if (stat(fullpath, &st) == 0) {
            dirs[ndirs].name = strdup(ent->d_name);
            dirs[ndirs].size = st.st_size;
            dirs[ndirs].mtime = st.st_mtime;
            dirs[ndirs].mode = st.st_mode;
            if (st.st_size > maxsize) maxsize = st.st_size;
            ndirs++;
        }
    }
    closedir(d);
    
    qsort(dirs, ndirs, sizeof(DirEntry), dircmp);
    
    sizew = maxsize == 0 ? 3 : 1 + (int)log10(maxsize) + 1;
    offset = 0;
}

void up(void) {
    char *p = strrchr(path, '/');
    if (p != NULL && p != path) {
        *p = '\0';
    } else if (strcmp(path, "/") != 0) {
        strcpy(path, "/");
    }
    loaddirs();
}

void cd(char *dir) {
    char newpath[PATH_MAX];
    
    if (dir == NULL) {
        strcpy(newpath, home);
    } else if (dir[0] == '/') {
        strcpy(newpath, dir);
    } else {
        snprintf(newpath, sizeof(newpath), "%s/%s", path, dir);
    }
    
    if (access(newpath, R_OK) == 0) {
        strcpy(path, newpath);
        loaddirs();
    } else {
        char errmsg[512];
        snprintf(errmsg, sizeof(errmsg), "Cannot access directory: %s", strerror(errno));
        alert("Directory Error", errmsg);
    }
}

void mkdir_vdir(char *name) {
    char fullpath[PATH_MAX];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);
    
    if (mkdir(fullpath, 0755) == 0) {
        loaddirs();
    } else {
        char errmsg[512];
        snprintf(errmsg, sizeof(errmsg), "Cannot create directory: %s", strerror(errno));
        alert("Create Directory Error", errmsg);
    }
}

void touch(char *name) {
    char fullpath[PATH_MAX];
    FILE *f;
    
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);
    f = fopen(fullpath, "w");
    if (f) {
        fclose(f);
        loaddirs();
    } else {
        char errmsg[512];
        snprintf(errmsg, sizeof(errmsg), "Cannot create file: %s", strerror(errno));
        alert("Create File Error", errmsg);
    }
}

void rm_file(DirEntry d) {
    char cmd[PATH_MAX + 20];
    char fullpath[PATH_MAX];
    
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, d.name);
    
    if (S_ISDIR(d.mode)) {
        snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", fullpath);
    } else {
        snprintf(cmd, sizeof(cmd), "rm \"%s\"", fullpath);
    }
    
    if (system(cmd) == 0) {
        loaddirs();
    } else {
        alert("Delete Error", "Cannot remove file/directory");
    }
}

void mv_file(char *from, char *to) {
    char cmd[PATH_MAX * 2 + 20];
    char frompath[PATH_MAX], topath[PATH_MAX];
    
    snprintf(frompath, sizeof(frompath), "%s/%s", path, from);
    snprintf(topath, sizeof(topath), "%s/%s", path, to);
    snprintf(cmd, sizeof(cmd), "mv \"%s\" \"%s\"", frompath, topath);
    
    if (system(cmd) == 0) {
        loaddirs();
    } else {
        alert("Rename Error", "Cannot rename file/directory");
    }
}

// Drawing functions
void draw_rect(Rectangle r, Color color) {
    XSetForeground(display, gc, color.pixel);
    XFillRectangle(display, window, gc, r.min.x, r.min.y, dx(r), dy(r));
}

void draw_border(Rectangle r, int width, Color color) {
    XSetForeground(display, gc, color.pixel);
    XSetLineAttributes(display, gc, width, LineSolid, CapButt, JoinMiter);
    XDrawRectangle(display, window, gc, r.min.x, r.min.y, dx(r) - 1, dy(r) - 1);
}

void draw_line(Point p1, Point p2, Color color) {
    XSetForeground(display, gc, color.pixel);
    XDrawLine(display, window, gc, p1.x, p1.y, p2.x, p2.y);
}

int text_width(char *text) {
    return XTextWidth(font, text, strlen(text));
}

void draw_text(Point p, Color color, char *text) {
    XSetForeground(display, gc, color.pixel);
    XDrawString(display, window, gc, p.x, p.y + font->ascent, text, strlen(text));
}

Point draw_text_truncated(Point p, Color color, char *text, int maxwidth) {
    int w = text_width(text);
    if (w <= maxwidth) {
        draw_text(p, color, text);
        return pt(p.x + w, p.y);
    }
    
    // Truncate with ellipsis
    char truncated[256];
    int ellipsis_w = text_width("...");
    int len = strlen(text);
    
    for (int i = len - 1; i > 0; i--) {
        strncpy(truncated, text, i);
        truncated[i] = '\0';
        strcat(truncated, "...");
        if (text_width(truncated) <= maxwidth) {
            draw_text(p, color, truncated);
            return pt(p.x + text_width(truncated), p.y);
        }
    }
    
    draw_text(p, color, "...");
    return pt(p.x + ellipsis_w, p.y);
}

Rectangle draw_button(Point *p, Color fg, Pixmap icon) {
    Rectangle r;
    p->x += Toolpadding;
    r = rect(p->x, p->y, p->x + 16, p->y + 16);
    
    if (icon != None) {
        XCopyArea(display, icon, window, gc, 0, 0, 16, 16, r.min.x, r.min.y);
    } else {
        draw_rect(r, fg);
    }
    
    p->x += 16 + Toolpadding;
    return r;
}

char *format_date(time_t t) {
    static char buf[64];
    struct tm *tm = localtime(&t);
    static char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    snprintf(buf, sizeof(buf), "%s %02d %02d:%02d", 
             months[tm->tm_mon], tm->tm_mday, tm->tm_hour, tm->tm_min);
    return buf;
}

void draw_dir(int n, int selected) {
    if (offset + n >= ndirs) return;
    
    DirEntry d = dirs[offset + n];
    Color bg = selected ? selbg : viewbg;
    Color fg = selected ? selfg : viewfg;
    
    Point p = addpt(viewr.min, pt(Toolpadding, Toolpadding));
    p.y += n * lineh;
    Rectangle r = rect(p.x, p.y, viewr.max.x - Toolpadding, p.y + lineh);
    
    // Draw background
    draw_rect(r, bg);
    
    // Format size and date
    char sizedate[128];
    char *datestr = format_date(d.mtime);
    snprintf(sizedate, sizeof(sizedate), "%*lld  %s", sizew, (long long)d.size, datestr);
    
    // Draw icon
    p.y -= Padding; // Reset y position for icon
    int icon_y_offset = (lineh - 12) / 2; // Center 12px icon in line height
    if (S_ISDIR(d.mode)) {
        XCopyArea(display, folder_icon, window, gc, 0, 0, 12, 12, p.x, p.y + icon_y_offset);
    } else {
        XCopyArea(display, file_icon, window, gc, 0, 0, 12, 12, p.x, p.y + icon_y_offset);
    }
    p.x += 16;
    p.y += (lineh - (font->ascent + font->descent)) / 2; // Reset for text
    
    // Draw filename
    int name_max_width = viewr.max.x - p.x - text_width(sizedate) - 2 * Padding - Toolpadding;
    p = draw_text_truncated(p, fg, d.name, name_max_width);
    
    // Draw size and date (right-aligned)
    p.x = viewr.max.x - text_width(sizedate) - Toolpadding;
    draw_text(p, fg, sizedate);
}

void redraw(void) {
    // Clear window
    XClearWindow(display, window);
    
    // Draw toolbar
    draw_rect(toolr, toolbg);
    draw_line(pt(toolr.min.x, toolr.max.y), pt(toolr.max.x, toolr.max.y), toolfg);
    
    // Draw toolbar buttons
    Point p = addpt(toolr.min, pt(0, Toolpadding));
    homer = draw_button(&p, toolfg, home_icon);
    cdr = draw_button(&p, toolfg, cd_icon);
    upr = draw_button(&p, toolfg, up_icon);
    
    // Draw path
    p.x += Toolpadding;
    p.y = toolr.min.y + (dy(toolr) - (font->ascent + font->descent)) / 2;
    pathr = rect(p.x, p.y, p.x + text_width(path), p.y + (font->ascent + font->descent));
    draw_text(p, toolfg, path);
    
    // Draw new file/folder buttons (right side)
    p.x = width - 2 * (Toolpadding + 16 + Toolpadding);
    p.y = toolr.min.y + Toolpadding;
    newdirr = draw_button(&p, toolfg, newfolder_icon);
    newfiler = draw_button(&p, toolfg, newfile_icon);
    
    // Draw scrollbar
    draw_rect(scrollr, scrollbg);
    if (ndirs > 0) {
        int h = (nlines * dy(scrollr)) / ndirs;
        int y = (offset * dy(scrollr)) / ndirs;
        scrposr = rect(scrollr.min.x, scrollr.min.y + y, scrollr.max.x - 1, scrollr.min.y + y + h);
    } else {
        scrposr = scrollr;
    }
    draw_rect(scrposr, scrollfg);
    
    // Draw directory entries
    for (int i = 0; i < nlines && offset + i < ndirs; i++) {
        draw_dir(i, 0);
    }
    
    XFlush(display);
}

void flash(int n) {
    for (int i = 0; i < 5; i++) {
        draw_dir(n, i & 1);
        XFlush(display);
        usleep(50000);
    }
}

// Event handling
int scrollclamp(int off) {
    if (nlines >= ndirs) return 0;
    if (off < 0) return 0;
    if (off + nlines > ndirs) return ndirs - nlines;
    return off;
}

void scrollup(int off) {
    int newoff = scrollclamp(offset - off);
    if (newoff != offset) {
        offset = newoff;
        redraw();
    }
}

void scrolldown(int off) {
    int newoff = scrollclamp(offset + off);
    if (newoff != offset) {
        offset = newoff;
        redraw();
    }
}

void handle_resize(int w, int h) {
    width = w;
    height = h;
    
    lineh = Padding + (font->ascent + font->descent) + Padding;
    toolr = rect(0, 0, width, 16 + 2 * Toolpadding);
    scrollr = rect(0, toolr.max.y + 1, Scrollwidth, height);
    viewr = rect(Scrollwidth, toolr.max.y + 1, width, height);
    nlines = dy(viewr) / lineh;
    
    redraw();
}

int indexat(Point p) {
    if (!ptinrect(p, viewr)) return -1;
    int n = (p.y - viewr.min.y) / lineh;
    if (offset + n >= ndirs) return -1;
    return n;
}

void handle_button_press(XButtonEvent *event) {
    Point p = pt(event->x, event->y);
    
    if (event->button == Button1) {
        if (ptinrect(p, homer)) {
            cd(NULL);
            redraw();
        } else if (ptinrect(p, upr)) {
            up();
            redraw();
        } else if (ptinrect(p, cdr)) {
            char dir[PATH_MAX];
            if (text_input("Go to directory", dir, sizeof(dir))) {
                cd(dir);
                redraw();
            }
        } else if (ptinrect(p, newdirr)) {
            char name[256] = {0};
            if (text_input("Create directory", name, sizeof(name))) {
                mkdir_vdir(name);
                redraw();
            }
        } else if (ptinrect(p, newfiler)) {
            char name[256] = {0};
            if (text_input("Create file", name, sizeof(name))) {
                touch(name);
                redraw();
            }
        } else if (ptinrect(p, viewr)) {
            int n = indexat(p);
            if (n >= 0) {
                DirEntry d = dirs[offset + n];
                if (S_ISDIR(d.mode)) {
                    cd(d.name);
                    redraw();
                } else {
                    // Open file with appropriate application
                    char cmd[PATH_MAX + 50];
                    if (is_image_file(d.name)) {
                        snprintf(cmd, sizeof(cmd), "%s \"%s/%s\" &", config->image_viewer_command, path, d.name);
                    } else {
                        snprintf(cmd, sizeof(cmd), "%s \"%s/%s\" &", config->editor_command, path, d.name);
                    }
                    system(cmd);
                    flash(n);
                }
            }
        }
    } else if (event->button == Button3) {
        if (ptinrect(p, viewr)) {
            int n = indexat(p);
            if (n >= 0) {
                DirEntry d = dirs[offset + n];
                char choice[64];
                char message[512];
                snprintf(message, sizeof(message), "File: %s - (d)elete or (r)ename?", d.name);
                
                if (text_input(message, choice, sizeof(choice))) {
                    if (choice[0] == 'd' || choice[0] == 'D') {
                        char confirm_msg[256];
                        snprintf(confirm_msg, sizeof(confirm_msg), "Delete %s?", d.name);
                        if (confirm(confirm_msg)) {
                            rm_file(d);
                            redraw();
                        }
                    } else if (choice[0] == 'r' || choice[0] == 'R') {
                        char newname[256] = {0};
                        if (text_input("Rename to", newname, sizeof(newname))) {
                            mv_file(d.name, newname);
                            redraw();
                        }
                    }
                }
            }
        }
    } else if (event->button == Button4) {
        scrollup(Slowscroll);
    } else if (event->button == Button5) {
        scrolldown(Slowscroll);
    }
}

void handle_key_press(XKeyEvent *event) {
    KeySym keysym = XLookupKeysym(event, 0);
    
    switch (keysym) {
    case XK_q:
    case XK_Delete:
        exit(0);
        break;
    case XK_Page_Up:
        scrollup(nlines);
        break;
    case XK_Page_Down:
        scrolldown(nlines);
        break;
    case XK_Home:
        cd(NULL);
        redraw();
        break;
    case XK_Up:
        up();
        redraw();
        break;
    case XK_space:
        printf("Current path: %s\n", path);
        break;
    }
}

void handle_motion(XMotionEvent *event) {
    Point p = pt(event->x, event->y);
    int n = indexat(p);
    
    if (n != lastn) {
        if (lastn >= 0) {
            draw_dir(lastn, 0);
        }
        if (n >= 0) {
            draw_dir(n, 1);
        }
        lastn = n;
        XFlush(display);
    }
}

int main(int argc, char *argv[]) {
    // Load configuration first
    config = load_config();
    if (!config) {
        fprintf(stderr, "Cannot load configuration\n");
        return 1;
    }
    
    // Set window size from config
    width = config->window_width;
    height = config->window_height;
    
    if (argc > 1) {
        if (realpath(argv[1], path) == NULL) {
            strcpy(path, argv[1]);
        }
    } else {
        if (getcwd(path, sizeof(path)) == NULL) {
            strcpy(path, "/");
        }
    }
    
    // Initialize X11
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }
    
    screen_num = DefaultScreen(display);
    colormap = DefaultColormap(display, screen_num);
    
    // Load font from config
    char font_pattern[256];
    snprintf(font_pattern, sizeof(font_pattern), "-*-%s-*-*-*-*-*-*-*-*-*-*-*-*", config->font_name);
    font = XLoadQueryFont(display, font_pattern);
    if (font == NULL) {
        font = XLoadQueryFont(display, "fixed");
        if (font == NULL) {
            font = XLoadQueryFont(display, "*");
            if (font == NULL) {
                fprintf(stderr, "Cannot load any font\n");
                return 1;
            }
        }
    }
    
    // Create window
    window = XCreateSimpleWindow(display, RootWindow(display, screen_num),
                                 0, 0, width, height, 1,
                                 BlackPixel(display, screen_num),
                                 WhitePixel(display, screen_num));
    
    XStoreName(display, window, "9dir");
    
    // Create GC
    gc = XCreateGC(display, window, 0, NULL);
    XSetFont(display, gc, font->fid);
    
    // Select events
    XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | 
                 StructureNotifyMask | PointerMotionMask);
    
    // Map window
    XMapWindow(display, window);
    
    // Initialize
    readhome();
    initcolors();
    init_icons(display, window, gc);
    loaddirs();
    handle_resize(width, height);
    
    // Event loop
    XEvent event;
    while (1) {
        XNextEvent(display, &event);
        
        switch (event.type) {
        case Expose:
            if (event.xexpose.count == 0) {
                redraw();
            }
            break;
        case ConfigureNotify:
            handle_resize(event.xconfigure.width, event.xconfigure.height);
            break;
        case KeyPress:
            handle_key_press(&event.xkey);
            break;
        case ButtonPress:
            handle_button_press(&event.xbutton);
            break;
        case MotionNotify:
            handle_motion(&event.xmotion);
            break;
        }
    }
    
    // Cleanup
    free_config(config);
    return 0;
}