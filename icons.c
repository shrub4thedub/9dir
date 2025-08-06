#include <X11/Xlib.h>
#include <stdlib.h>
#include "icons.h"

Pixmap home_icon = None;
Pixmap folder_icon = None;
Pixmap file_icon = None;
Pixmap up_icon = None;
Pixmap cd_icon = None;
Pixmap newfile_icon = None;
Pixmap newfolder_icon = None;

// Better geometric icons inspired by Plan 9 design
void init_icons(Display *display, Window window, GC gc) {
    int screen = DefaultScreen(display);
    
    // Create pixmaps - 16x16 for toolbar, 12x12 for folder/file listing
    home_icon = XCreatePixmap(display, window, 16, 16, DefaultDepth(display, screen));
    folder_icon = XCreatePixmap(display, window, 12, 12, DefaultDepth(display, screen));
    file_icon = XCreatePixmap(display, window, 12, 12, DefaultDepth(display, screen));
    up_icon = XCreatePixmap(display, window, 16, 16, DefaultDepth(display, screen));
    cd_icon = XCreatePixmap(display, window, 16, 16, DefaultDepth(display, screen));
    newfile_icon = XCreatePixmap(display, window, 16, 16, DefaultDepth(display, screen));
    newfolder_icon = XCreatePixmap(display, window, 16, 16, DefaultDepth(display, screen));
    
    // Clear all pixmaps with light gray background
    XSetForeground(display, gc, 0xEFEFEF);
    XFillRectangle(display, home_icon, gc, 0, 0, 16, 16);
    XFillRectangle(display, folder_icon, gc, 0, 0, 12, 12);
    XFillRectangle(display, file_icon, gc, 0, 0, 12, 12);
    XFillRectangle(display, up_icon, gc, 0, 0, 16, 16);
    XFillRectangle(display, cd_icon, gc, 0, 0, 16, 16);
    XFillRectangle(display, newfile_icon, gc, 0, 0, 16, 16);
    XFillRectangle(display, newfolder_icon, gc, 0, 0, 16, 16);
    
    // Draw with dark colors
    XSetForeground(display, gc, BlackPixel(display, screen));
    XSetLineAttributes(display, gc, 1, LineSolid, CapButt, JoinMiter);
    
    // Home icon - house shape
    XPoint house[] = {{8,1}, {1,7}, {3,7}, {3,14}, {6,14}, {6,10}, {10,10}, {10,14}, {13,14}, {13,7}, {15,7}, {8,1}};
    XDrawLines(display, home_icon, gc, house, 12, CoordModeOrigin);
    XDrawRectangle(display, home_icon, gc, 7, 9, 2, 5);
    
    // Folder icon - folder shape (12x12)
    XFillRectangle(display, folder_icon, gc, 1, 5, 10, 6);
    XFillRectangle(display, folder_icon, gc, 1, 3, 4, 2);
    XSetForeground(display, gc, 0xFFFFFF);
    XFillRectangle(display, folder_icon, gc, 2, 6, 8, 4);
    XSetForeground(display, gc, BlackPixel(display, screen));
    
    // File icon - document shape (12x12)
    XFillRectangle(display, file_icon, gc, 2, 1, 7, 10);
    XSetForeground(display, gc, 0xFFFFFF);
    XFillRectangle(display, file_icon, gc, 3, 2, 5, 8);
    XSetForeground(display, gc, BlackPixel(display, screen));
    XDrawLine(display, file_icon, gc, 4, 4, 7, 4);
    XDrawLine(display, file_icon, gc, 4, 6, 7, 6);
    XDrawLine(display, file_icon, gc, 4, 8, 6, 8);
    
    // Up icon - arrow pointing up
    XPoint up_arrow[] = {{8,2}, {4,8}, {6,8}, {6,13}, {10,13}, {10,8}, {12,8}, {8,2}};
    XFillPolygon(display, up_icon, gc, up_arrow, 8, Complex, CoordModeOrigin);
    
    // CD icon - arrow pointing right  
    XPoint right_arrow[] = {{2,8}, {8,4}, {8,6}, {13,6}, {13,10}, {8,10}, {8,12}, {2,8}};
    XFillPolygon(display, cd_icon, gc, right_arrow, 8, Complex, CoordModeOrigin);
    
    // New file icon - document with plus
    XFillRectangle(display, newfile_icon, gc, 2, 1, 7, 10);
    XSetForeground(display, gc, 0xFFFFFF);
    XFillRectangle(display, newfile_icon, gc, 3, 2, 5, 8);
    XSetForeground(display, gc, 0x00AA00);
    XFillRectangle(display, newfile_icon, gc, 5, 12, 1, 3);
    XFillRectangle(display, newfile_icon, gc, 4, 13, 3, 1);
    XSetForeground(display, gc, BlackPixel(display, screen));
    
    // New folder icon - folder with plus
    XFillRectangle(display, newfolder_icon, gc, 1, 5, 10, 6);
    XFillRectangle(display, newfolder_icon, gc, 1, 3, 4, 2);
    XSetForeground(display, gc, 0x00AA00);
    XFillRectangle(display, newfolder_icon, gc, 12, 7, 1, 3);
    XFillRectangle(display, newfolder_icon, gc, 11, 8, 3, 1);
}