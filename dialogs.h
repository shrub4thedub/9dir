#ifndef DIALOGS_LINUX_H
#define DIALOGS_LINUX_H

#include <X11/Xlib.h>
#include "theme.h"

void alert(const char *message, const char *err);
int confirm(const char *message);
int text_input(const char *prompt, char *buffer, size_t bufsize);
void about_dialog(void);

#endif