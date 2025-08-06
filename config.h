#ifndef CONFIG_H
#define CONFIG_H

#include <X11/Xlib.h>
#include "theme.h"

typedef struct Config Config;

struct Config {
    // Font settings
    char *font_name;
    int font_size;
    
    // Colors (hex format)
    char *bg_color;
    char *fg_color;
    char *toolbar_bg;
    char *toolbar_fg;
    char *selected_bg;
    char *selected_fg;
    char *scrollbar_bg;
    char *scrollbar_fg;
    char *highlight_color;
    
    // Editor settings
    char *editor_command;
    char *image_viewer_command;
    
    // Window settings
    int window_width;
    int window_height;
    
    // Behavior settings
    int show_hidden_files;
    int double_click_to_open;
    
    // Icon settings
    int show_icons;
    char *icon_theme;
};

// Function declarations
Config* load_config(void);
void free_config(Config *config);
void create_default_config(void);
Color parse_color_config(Display *display, Colormap colormap, const char *color_str);
int is_image_file(const char *filename);

#endif