#ifndef THEME_LINUX_H
#define THEME_LINUX_H

#include <X11/Xlib.h>

typedef struct Color Color;
typedef struct Theme Theme;

struct Color {
    unsigned long pixel;
    XColor xcolor;
};

struct Theme {
    Color back;
    Color high;
    Color border;
    Color text;
    Color htext;
    Color title;
    Color ltitle;
    Color hold;
    Color lhold;
    Color palehold;
    Color paletext;
    Color size;
    Color menubar;
    Color menuback;
    Color menuhigh;
    Color menubord;
    Color menutext;
    Color menuhtext;
};

Theme* loadtheme(void);

#endif