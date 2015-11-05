/**
 * Author: Viacheslav Lotsmanov
 * License: GNU/GPLv3 https://raw.githubusercontent.com/unclechu/xlib-keyboard-hack/master/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>

int main(int argc, char **argv)
{
	Display *dpy = XOpenDisplay(NULL);
	Window wnd = DefaultRootWindow(dpy);
	
	const int hack_key_num = 37;
	
	int last_hack_key_state = 0;
	char keys_return[32];
	while (1) {
		
		XQueryKeymap(dpy, keys_return);
		int cur_hack_key_state = 0;
		
		for (int i=0; i<32; i++) {
			
			if (keys_return[i] != 0) {
				
				int pos = 0;
				int num = keys_return[i];
				
				while (pos < 8) {
					
					if ((num & 0x01) == 1) {
						int key_num = i*8+pos;
						if (key_num == hack_key_num) {
							cur_hack_key_state = 1;
						}
					}
					
					++pos;
					num /= 2;
				}
			}
		}
		
		if (cur_hack_key_state != last_hack_key_state) {
			last_hack_key_state = cur_hack_key_state;
			
			if (last_hack_key_state == 1) {
				
				system("xmodmap -e 'keycode 113 = Home'");
				system("xmodmap -e 'keycode 114 = End'");
				system("xmodmap -e 'keycode 111 = Prior'");
				system("xmodmap -e 'keycode 116 = Next'");
				
				system("xmodmap -e 'keycode 105 = Menu'");
				
				printf("On\n");
				
			} else {
				
				system("xmodmap -e 'keycode 113 = Left'");
				system("xmodmap -e 'keycode 114 = Right'");
				system("xmodmap -e 'keycode 111 = Up'");
				system("xmodmap -e 'keycode 116 = Down'");
				
				system("xmodmap -e 'keycode 105 = Insert'");
				
				printf("Off\n");
			}
		}
		
		usleep(30000); // idle
	}
	
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
