/**
 * Author: Viacheslav Lotsmanov
 * License: GNU/GPLv3 https://raw.githubusercontent.com/unclechu/xlib-keyboard-hack/master/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

int main(int argc, char **argv)
{
	Display *dpy = XOpenDisplay(0);
	Window wnd = DefaultRootWindow(dpy);
	
	int keycode = XKeysymToKeycode(dpy, XK_Super_L);
	int last_status = 0;
	
	XGrabKey(dpy, keycode, 0, wnd, 1, GrabModeAsync, GrabModeAsync);
	
	XEvent ev;
	while (1) {
		
		XSelectInput(dpy, wnd, KeyPressMask);
		XNextEvent(dpy, &ev);
		
		if (ev.type != KeyPress || ev.xkey.keycode != keycode) {
			continue;
		}
		
		if (last_status == 0) {
			
			system("xmodmap -e 'keycode 113 = Home'");
			system("xmodmap -e 'keycode 114 = End'");
			system("xmodmap -e 'keycode 111 = Prior'");
			system("xmodmap -e 'keycode 116 = Next'");
			
			system("xmodmap -e 'keycode 105 = Menu'");
			
			last_status = 1;
			
		} else {
			
			system("xmodmap -e 'keycode 113 = Left'");
			system("xmodmap -e 'keycode 114 = Right'");
			system("xmodmap -e 'keycode 111 = Up'");
			system("xmodmap -e 'keycode 116 = Down'");
			
			system("xmodmap -e 'keycode 105 = Insert'");
			
			last_status = 0;
		}
	}
	
	return EXIT_SUCCESS;
}
