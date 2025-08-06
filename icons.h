#ifndef ICONS_LINUX_H
#define ICONS_LINUX_H

#include <X11/Xlib.h>

// Simple placeholder for icons - in a full implementation,
// these would be loaded from PNG/XPM files or embedded data
extern Pixmap home_icon;
extern Pixmap folder_icon;
extern Pixmap file_icon;
extern Pixmap up_icon;
extern Pixmap cd_icon;
extern Pixmap newfile_icon;
extern Pixmap newfolder_icon;

// Initialize icon pixmaps
void init_icons(Display *display, Window window, GC gc);

#endif