#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include "dialogs.h"

extern Display *display;
extern Window window;
extern GC gc;
extern XFontStruct *font;
extern int screen_num;

void alert(const char *message, const char *err) {
    printf("ALERT: %s", message);
    if (err != NULL) {
        printf(": %s", err);
    }
    printf("\nPress Enter to continue...");
    fflush(stdout);
    getchar();
}

int confirm(const char *message) {
    Window dialog_window;
    XEvent event;
    int dialog_width = 350;
    int dialog_height = 120;
    int screen_width = DisplayWidth(display, screen_num);
    int screen_height = DisplayHeight(display, screen_num);
    int x = (screen_width - dialog_width) / 2;
    int y = (screen_height - dialog_height) / 2;
    
    // Create modal dialog window
    dialog_window = XCreateSimpleWindow(display, RootWindow(display, screen_num),
                                      x, y, dialog_width, dialog_height, 1,
                                      BlackPixel(display, screen_num),
                                      WhitePixel(display, screen_num));
    
    // Set window properties
    XStoreName(display, dialog_window, "Confirm");
    XSelectInput(display, dialog_window, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(display, dialog_window);
    
    // Make it modal
    XSetTransientForHint(display, dialog_window, window);
    
    int done = 0;
    int result = 0;
    
    // Button rectangles
    int button_width = 60;
    int button_height = 25;
    int yes_x = dialog_width/2 - button_width - 10;
    int no_x = dialog_width/2 + 10;
    int button_y = dialog_height - 40;
    
    while (!done) {
        XNextEvent(display, &event);
        
        if (event.xany.window != dialog_window) continue;
        
        switch (event.type) {
            case Expose:
                XClearWindow(display, dialog_window);
                
                // Draw message
                XDrawString(display, dialog_window, gc, 10, 30, message, strlen(message));
                
                // Draw Yes button
                XDrawRectangle(display, dialog_window, gc, yes_x, button_y, button_width, button_height);
                XDrawString(display, dialog_window, gc, yes_x + 20, button_y + 17, "Yes", 3);
                
                // Draw No button  
                XDrawRectangle(display, dialog_window, gc, no_x, button_y, button_width, button_height);
                XDrawString(display, dialog_window, gc, no_x + 23, button_y + 17, "No", 2);
                
                break;
                
            case ButtonPress:
                if (event.xbutton.button == Button1) {
                    int click_x = event.xbutton.x;
                    int click_y = event.xbutton.y;
                    
                    // Check Yes button
                    if (click_x >= yes_x && click_x <= yes_x + button_width &&
                        click_y >= button_y && click_y <= button_y + button_height) {
                        result = 1;
                        done = 1;
                    }
                    // Check No button
                    else if (click_x >= no_x && click_x <= no_x + button_width &&
                            click_y >= button_y && click_y <= button_y + button_height) {
                        result = 0;
                        done = 1;
                    }
                }
                break;
                
            case KeyPress: {
                KeySym keysym;
                char key_buffer[32];
                XLookupString(&event.xkey, key_buffer, sizeof(key_buffer), &keysym, NULL);
                
                if (keysym == XK_y || keysym == XK_Y) {
                    result = 1;
                    done = 1;
                } else if (keysym == XK_n || keysym == XK_N || keysym == XK_Escape) {
                    result = 0;
                    done = 1;
                } else if (keysym == XK_Return) {
                    result = 0;  // Default to No on Enter
                    done = 1;
                }
                break;
            }
        }
    }
    
    XDestroyWindow(display, dialog_window);
    XFlush(display);
    
    return result;
}

int text_input(const char *prompt, char *buffer, size_t bufsize) {
    Window dialog_window;
    XEvent event;
    int dialog_width = 400;
    int dialog_height = 100;
    int screen_width = DisplayWidth(display, screen_num);
    int screen_height = DisplayHeight(display, screen_num);
    int x = (screen_width - dialog_width) / 2;
    int y = (screen_height - dialog_height) / 2;
    
    // Create modal dialog window
    dialog_window = XCreateSimpleWindow(display, RootWindow(display, screen_num),
                                      x, y, dialog_width, dialog_height, 1,
                                      BlackPixel(display, screen_num),
                                      WhitePixel(display, screen_num));
    
    // Set window properties
    XStoreName(display, dialog_window, "Input");
    XSelectInput(display, dialog_window, ExposureMask | KeyPressMask);
    XMapWindow(display, dialog_window);
    
    // Make it modal
    XSetTransientForHint(display, dialog_window, window);
    
    char input_text[256] = {0};
    int cursor_pos = 0;
    int done = 0;
    int result = 0;
    
    while (!done) {
        XNextEvent(display, &event);
        
        if (event.xany.window != dialog_window) continue;
        
        switch (event.type) {
            case Expose:
                XClearWindow(display, dialog_window);
                
                // Draw prompt
                XDrawString(display, dialog_window, gc, 10, 25, prompt, strlen(prompt));
                
                // Draw input box
                XDrawRectangle(display, dialog_window, gc, 10, 35, dialog_width - 20, 25);
                
                // Draw current input text
                if (strlen(input_text) > 0) {
                    XDrawString(display, dialog_window, gc, 15, 52, input_text, strlen(input_text));
                }
                
                // Draw cursor
                int text_width = XTextWidth(font, input_text, cursor_pos);
                XDrawLine(display, dialog_window, gc, 15 + text_width, 40, 15 + text_width, 57);
                
                break;
                
            case KeyPress: {
                KeySym keysym;
                char key_buffer[32];
                int len = XLookupString(&event.xkey, key_buffer, sizeof(key_buffer), &keysym, NULL);
                
                if (keysym == XK_Return) {
                    strncpy(buffer, input_text, bufsize - 1);
                    buffer[bufsize - 1] = '\0';
                    result = strlen(input_text) > 0 ? 1 : 0;
                    done = 1;
                } else if (keysym == XK_Escape) {
                    result = 0;
                    done = 1;
                } else if (keysym == XK_BackSpace) {
                    if (cursor_pos > 0) {
                        memmove(&input_text[cursor_pos - 1], &input_text[cursor_pos], 
                               strlen(input_text) - cursor_pos + 1);
                        cursor_pos--;
                    }
                } else if (keysym == XK_Left) {
                    if (cursor_pos > 0) cursor_pos--;
                } else if (keysym == XK_Right) {
                    if (cursor_pos < (int)strlen(input_text)) cursor_pos++;
                } else if (len > 0 && key_buffer[0] >= 32 && key_buffer[0] < 127) {
                    // Insert printable character
                    if (strlen(input_text) < sizeof(input_text) - 1) {
                        memmove(&input_text[cursor_pos + 1], &input_text[cursor_pos], 
                               strlen(input_text) - cursor_pos + 1);
                        input_text[cursor_pos] = key_buffer[0];
                        cursor_pos++;
                    }
                }
                
                // Redraw
                XClearWindow(display, dialog_window);
                XDrawString(display, dialog_window, gc, 10, 25, prompt, strlen(prompt));
                XDrawRectangle(display, dialog_window, gc, 10, 35, dialog_width - 20, 25);
                if (strlen(input_text) > 0) {
                    XDrawString(display, dialog_window, gc, 15, 52, input_text, strlen(input_text));
                }
                int text_width = XTextWidth(font, input_text, cursor_pos);
                XDrawLine(display, dialog_window, gc, 15 + text_width, 40, 15 + text_width, 57);
                
                break;
            }
        }
    }
    
    XDestroyWindow(display, dialog_window);
    XFlush(display);
    
    return result;
}

void about_dialog(void) {
    Window dialog_window;
    XEvent event;
    int dialog_width = 400;
    int dialog_height = 180;
    int screen_width = DisplayWidth(display, screen_num);
    int screen_height = DisplayHeight(display, screen_num);
    int x = (screen_width - dialog_width) / 2;
    int y = (screen_height - dialog_height) / 2;
    
    // Create modal dialog window
    dialog_window = XCreateSimpleWindow(display, RootWindow(display, screen_num),
                                      x, y, dialog_width, dialog_height, 1,
                                      BlackPixel(display, screen_num),
                                      WhitePixel(display, screen_num));
    
    // Set window properties
    XStoreName(display, dialog_window, "About 9dir");
    XSelectInput(display, dialog_window, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(display, dialog_window);
    
    // Make it modal
    XSetTransientForHint(display, dialog_window, window);
    
    int done = 0;
    
    // Button rectangle
    int button_width = 80;
    int button_height = 25;
    int button_x = (dialog_width - button_width) / 2;
    int button_y = dialog_height - 40;
    
    const char *line1 = "9dir 0.1";
    const char *line2 = "by shrub industries";
    const char *line3 = "This software is provided to you under the terms";
    const char *line4 = "of the GNU GPL V3 license.";
    
    while (!done) {
        XNextEvent(display, &event);
        
        if (event.xany.window != dialog_window) continue;
        
        switch (event.type) {
            case Expose:
                XClearWindow(display, dialog_window);
                
                // Draw version and author info centered
                int line1_w = XTextWidth(font, line1, strlen(line1));
                int line2_w = XTextWidth(font, line2, strlen(line2));
                int line3_w = XTextWidth(font, line3, strlen(line3));
                int line4_w = XTextWidth(font, line4, strlen(line4));
                
                XDrawString(display, dialog_window, gc, (dialog_width - line1_w) / 2, 30, line1, strlen(line1));
                XDrawString(display, dialog_window, gc, (dialog_width - line2_w) / 2, 50, line2, strlen(line2));
                XDrawString(display, dialog_window, gc, (dialog_width - line3_w) / 2, 80, line3, strlen(line3));
                XDrawString(display, dialog_window, gc, (dialog_width - line4_w) / 2, 100, line4, strlen(line4));
                
                // Draw OK button
                XDrawRectangle(display, dialog_window, gc, button_x, button_y, button_width, button_height);
                XDrawString(display, dialog_window, gc, button_x + 35, button_y + 17, "OK", 2);
                
                break;
                
            case ButtonPress:
                if (event.xbutton.button == Button1) {
                    int click_x = event.xbutton.x;
                    int click_y = event.xbutton.y;
                    
                    // Check OK button
                    if (click_x >= button_x && click_x <= button_x + button_width &&
                        click_y >= button_y && click_y <= button_y + button_height) {
                        done = 1;
                    }
                }
                break;
                
            case KeyPress: {
                KeySym keysym;
                char key_buffer[32];
                XLookupString(&event.xkey, key_buffer, sizeof(key_buffer), &keysym, NULL);
                
                if (keysym == XK_Return || keysym == XK_Escape || keysym == XK_space) {
                    done = 1;
                }
                break;
            }
        }
    }
    
    XDestroyWindow(display, dialog_window);
    XFlush(display);
}