#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include "theme.h"

extern Display *display;
extern int screen_num;
extern Colormap colormap;

Color alloccolor_theme(unsigned long rgb) {
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

Color parsecolor(char *s) {
    unsigned long c = strtoul(s, NULL, 16);
    return alloccolor_theme(c);
}

Theme* loadtheme(void) {
    Theme *theme;
    FILE *fp;
    char line[256];
    char *home = getenv("HOME");
    char themepath[512];
    
    if (home == NULL) return NULL;
    
    snprintf(themepath, sizeof(themepath), "%s/.config/9dir/theme", home);
    
    if (access(themepath, R_OK) != 0) {
        // Try system theme
        strcpy(themepath, "/etc/9dir/theme");
        if (access(themepath, R_OK) != 0) {
            return NULL;
        }
    }
    
    fp = fopen(themepath, "r");
    if (fp == NULL) return NULL;
    
    theme = calloc(1, sizeof(Theme));
    if (theme == NULL) {
        fclose(fp);
        return NULL;
    }
    
    while (fgets(line, sizeof(line), fp)) {
        char *key = strtok(line, " \t\n");
        char *value = strtok(NULL, " \t\n");
        
        if (key == NULL || value == NULL) continue;
        
        if (strcmp(key, "back") == 0) {
            theme->back = parsecolor(value);
        } else if (strcmp(key, "high") == 0) {
            theme->high = parsecolor(value);
        } else if (strcmp(key, "border") == 0) {
            theme->border = parsecolor(value);
        } else if (strcmp(key, "text") == 0) {
            theme->text = parsecolor(value);
        } else if (strcmp(key, "htext") == 0) {
            theme->htext = parsecolor(value);
        } else if (strcmp(key, "title") == 0) {
            theme->title = parsecolor(value);
        } else if (strcmp(key, "ltitle") == 0) {
            theme->ltitle = parsecolor(value);
        } else if (strcmp(key, "hold") == 0) {
            theme->hold = parsecolor(value);
        } else if (strcmp(key, "lhold") == 0) {
            theme->lhold = parsecolor(value);
        } else if (strcmp(key, "palehold") == 0) {
            theme->palehold = parsecolor(value);
        } else if (strcmp(key, "paletext") == 0) {
            theme->paletext = parsecolor(value);
        } else if (strcmp(key, "size") == 0) {
            theme->size = parsecolor(value);
        } else if (strcmp(key, "menuback") == 0) {
            theme->menuback = parsecolor(value);
        } else if (strcmp(key, "menuhigh") == 0) {
            theme->menuhigh = parsecolor(value);
        } else if (strcmp(key, "menubord") == 0) {
            theme->menubord = parsecolor(value);
        } else if (strcmp(key, "menutext") == 0) {
            theme->menutext = parsecolor(value);
        } else if (strcmp(key, "menuhtext") == 0) {
            theme->menuhtext = parsecolor(value);
        }
    }
    
    fclose(fp);
    return theme;
}