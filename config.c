#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include "config.h"

#define MAX_LINE 1024
#define MAX_KEY 128
#define MAX_VALUE 512

// Default configuration values
static Config default_config = {
    .font_name = "sleekspace",
    .font_size = 12,
    .bg_color = "#FFFFFF",
    .fg_color = "#000000", 
    .toolbar_bg = "#EFEFEF",
    .toolbar_fg = "#000000",
    .selected_bg = "#EFEFEF",
    .selected_fg = "#000000",
    .scrollbar_bg = "#999999",
    .scrollbar_fg = "#FFFFFF",
    .highlight_color = "#00AA00",
    .editor_command = "acme",
    .image_viewer_command = "feh",
    .window_width = 800,
    .window_height = 600,
    .show_hidden_files = 0,
    .double_click_to_open = 0,
    .show_icons = 1,
    .icon_theme = "default"
};

static char* trim_whitespace(char *str) {
    char *end;
    
    // Trim leading space
    while (*str == ' ' || *str == '\t') str++;
    
    if (*str == 0) return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) end--;
    
    // Write new null terminator
    *(end + 1) = 0;
    
    return str;
}

static char* config_path(void) {
    static char path[1024];
    char *home = getenv("HOME");
    
    if (!home) {
        strcpy(path, "/tmp/9dir-config");
        return path;
    }
    
    snprintf(path, sizeof(path), "%s/.config/9dir", home);
    return path;
}

static char* config_file_path(void) {
    static char path[1024];
    snprintf(path, sizeof(path), "%s/config", config_path());
    return path;
}

void create_default_config(void) {
    char *dir_path = config_path();
    char *file_path = config_file_path();
    FILE *f;
    
    // Create config directory if it doesn't exist
    struct stat st;
    if (stat(dir_path, &st) == -1) {
        if (mkdir(dir_path, 0755) == -1) {
            fprintf(stderr, "Warning: Cannot create config directory %s: %s\n", 
                    dir_path, strerror(errno));
            return;
        }
    }
    
    // Create default config file
    f = fopen(file_path, "w");
    if (!f) {
        fprintf(stderr, "Warning: Cannot create config file %s: %s\n", 
                file_path, strerror(errno));
        return;
    }
    
    fprintf(f, "# 9dir Configuration File\n");
    fprintf(f, "# Colors are in hex format (#RRGGBB)\n\n");
    
    fprintf(f, "# Font settings\n");
    fprintf(f, "font_name = %s\n", default_config.font_name);
    fprintf(f, "font_size = %d\n\n", default_config.font_size);
    
    fprintf(f, "# Color scheme\n");
    fprintf(f, "bg_color = %s\n", default_config.bg_color);
    fprintf(f, "fg_color = %s\n", default_config.fg_color);
    fprintf(f, "toolbar_bg = %s\n", default_config.toolbar_bg);
    fprintf(f, "toolbar_fg = %s\n", default_config.toolbar_fg);
    fprintf(f, "selected_bg = %s\n", default_config.selected_bg);
    fprintf(f, "selected_fg = %s\n", default_config.selected_fg);
    fprintf(f, "scrollbar_bg = %s\n", default_config.scrollbar_bg);
    fprintf(f, "scrollbar_fg = %s\n", default_config.scrollbar_fg);
    fprintf(f, "highlight_color = %s\n\n", default_config.highlight_color);
    
    fprintf(f, "# Editor settings\n");
    fprintf(f, "editor_command = %s\n", default_config.editor_command);
    fprintf(f, "image_viewer_command = %s\n\n", default_config.image_viewer_command);
    
    fprintf(f, "# Window settings\n");
    fprintf(f, "window_width = %d\n", default_config.window_width);
    fprintf(f, "window_height = %d\n\n", default_config.window_height);
    
    fprintf(f, "# Behavior settings\n");
    fprintf(f, "show_hidden_files = %d\n", default_config.show_hidden_files);
    fprintf(f, "double_click_to_open = %d\n\n", default_config.double_click_to_open);
    
    fprintf(f, "# Icon settings\n");
    fprintf(f, "show_icons = %d\n", default_config.show_icons);
    fprintf(f, "icon_theme = %s\n", default_config.icon_theme);
    
    fclose(f);
    printf("Created default config file at %s\n", file_path);
}

Config* load_config(void) {
    Config *config;
    FILE *f;
    char line[MAX_LINE];
    char key[MAX_KEY], value[MAX_VALUE];
    char *file_path = config_file_path();
    
    // Allocate config structure
    config = malloc(sizeof(Config));
    if (!config) return NULL;
    
    // Start with default values
    *config = default_config;
    
    // Duplicate string fields
    config->font_name = strdup(default_config.font_name);
    config->bg_color = strdup(default_config.bg_color);
    config->fg_color = strdup(default_config.fg_color);
    config->toolbar_bg = strdup(default_config.toolbar_bg);
    config->toolbar_fg = strdup(default_config.toolbar_fg);
    config->selected_bg = strdup(default_config.selected_bg);
    config->selected_fg = strdup(default_config.selected_fg);
    config->scrollbar_bg = strdup(default_config.scrollbar_bg);
    config->scrollbar_fg = strdup(default_config.scrollbar_fg);
    config->highlight_color = strdup(default_config.highlight_color);
    config->editor_command = strdup(default_config.editor_command);
    config->image_viewer_command = strdup(default_config.image_viewer_command);
    config->icon_theme = strdup(default_config.icon_theme);
    
    // Try to open config file
    f = fopen(file_path, "r");
    if (!f) {
        // Config file doesn't exist, create it
        create_default_config();
        return config;
    }
    
    // Parse config file
    while (fgets(line, sizeof(line), f)) {
        char *trimmed = trim_whitespace(line);
        
        // Skip empty lines and comments
        if (strlen(trimmed) == 0 || trimmed[0] == '#') continue;
        
        // Parse key = value
        if (sscanf(trimmed, "%127s = %511[^\n]", key, value) == 2) {
            char *trimmed_value = trim_whitespace(value);
            
            if (strcmp(key, "font_name") == 0) {
                free(config->font_name);
                config->font_name = strdup(trimmed_value);
            } else if (strcmp(key, "font_size") == 0) {
                config->font_size = atoi(trimmed_value);
            } else if (strcmp(key, "bg_color") == 0) {
                free(config->bg_color);
                config->bg_color = strdup(trimmed_value);
            } else if (strcmp(key, "fg_color") == 0) {
                free(config->fg_color);
                config->fg_color = strdup(trimmed_value);
            } else if (strcmp(key, "toolbar_bg") == 0) {
                free(config->toolbar_bg);
                config->toolbar_bg = strdup(trimmed_value);
            } else if (strcmp(key, "toolbar_fg") == 0) {
                free(config->toolbar_fg);
                config->toolbar_fg = strdup(trimmed_value);
            } else if (strcmp(key, "selected_bg") == 0) {
                free(config->selected_bg);
                config->selected_bg = strdup(trimmed_value);
            } else if (strcmp(key, "selected_fg") == 0) {
                free(config->selected_fg);
                config->selected_fg = strdup(trimmed_value);
            } else if (strcmp(key, "scrollbar_bg") == 0) {
                free(config->scrollbar_bg);
                config->scrollbar_bg = strdup(trimmed_value);
            } else if (strcmp(key, "scrollbar_fg") == 0) {
                free(config->scrollbar_fg);
                config->scrollbar_fg = strdup(trimmed_value);
            } else if (strcmp(key, "highlight_color") == 0) {
                free(config->highlight_color);
                config->highlight_color = strdup(trimmed_value);
            } else if (strcmp(key, "editor_command") == 0) {
                free(config->editor_command);
                config->editor_command = strdup(trimmed_value);
            } else if (strcmp(key, "image_viewer_command") == 0) {
                free(config->image_viewer_command);
                config->image_viewer_command = strdup(trimmed_value);
            } else if (strcmp(key, "window_width") == 0) {
                config->window_width = atoi(trimmed_value);
            } else if (strcmp(key, "window_height") == 0) {
                config->window_height = atoi(trimmed_value);
            } else if (strcmp(key, "show_hidden_files") == 0) {
                config->show_hidden_files = atoi(trimmed_value);
            } else if (strcmp(key, "double_click_to_open") == 0) {
                config->double_click_to_open = atoi(trimmed_value);
            } else if (strcmp(key, "show_icons") == 0) {
                config->show_icons = atoi(trimmed_value);
            } else if (strcmp(key, "icon_theme") == 0) {
                free(config->icon_theme);
                config->icon_theme = strdup(trimmed_value);
            }
        }
    }
    
    fclose(f);
    return config;
}

void free_config(Config *config) {
    if (!config) return;
    
    free(config->font_name);
    free(config->bg_color);
    free(config->fg_color);
    free(config->toolbar_bg);
    free(config->toolbar_fg);
    free(config->selected_bg);
    free(config->selected_fg);
    free(config->scrollbar_bg);
    free(config->scrollbar_fg);
    free(config->highlight_color);
    free(config->editor_command);
    free(config->image_viewer_command);
    free(config->icon_theme);
    free(config);
}

Color parse_color_config(Display *display, Colormap colormap, const char *color_str) {
    Color c;
    unsigned int r, g, b;
    
    // Parse hex color (#RRGGBB)
    if (color_str[0] == '#' && strlen(color_str) == 7) {
        if (sscanf(color_str + 1, "%2x%2x%2x", &r, &g, &b) == 3) {
            c.xcolor.red = r << 8;
            c.xcolor.green = g << 8;
            c.xcolor.blue = b << 8;
            c.xcolor.flags = DoRed | DoGreen | DoBlue;
            
            if (XAllocColor(display, colormap, &c.xcolor)) {
                c.pixel = c.xcolor.pixel;
            } else {
                c.pixel = BlackPixel(display, DefaultScreen(display));
            }
        } else {
            c.pixel = BlackPixel(display, DefaultScreen(display));
        }
    } else {
        c.pixel = BlackPixel(display, DefaultScreen(display));
    }
    
    return c;
}

int is_image_file(const char *filename) {
    if (!filename) return 0;
    
    // Find the last dot for extension
    char *ext = strrchr(filename, '.');
    if (!ext) return 0;
    
    // Convert to lowercase for comparison
    char ext_lower[16];
    int i = 0;
    while (ext[i] && i < 15) {
        ext_lower[i] = tolower(ext[i]);
        i++;
    }
    ext_lower[i] = '\0';
    
    // Check common image extensions
    return (strcmp(ext_lower, ".jpg") == 0 ||
            strcmp(ext_lower, ".jpeg") == 0 ||
            strcmp(ext_lower, ".png") == 0 ||
            strcmp(ext_lower, ".gif") == 0 ||
            strcmp(ext_lower, ".bmp") == 0 ||
            strcmp(ext_lower, ".tiff") == 0 ||
            strcmp(ext_lower, ".tif") == 0 ||
            strcmp(ext_lower, ".webp") == 0 ||
            strcmp(ext_lower, ".svg") == 0 ||
            strcmp(ext_lower, ".ico") == 0);
}