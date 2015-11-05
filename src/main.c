/**
 * Author: Viacheslav Lotsmanov
 * License: GNU/GPLv3 https://raw.githubusercontent.com/unclechu/xlib-keyboard-hack/master/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <X11/Xlib.h>

#define APPNAME "xlib-keyboard-hack"

int main(int argc, char **argv)
{
	const char *home_dir = getenv("HOME");
	
	// paths to files
	// TODO cache scripts contents to prevent reading from fs every times
	char hack_key_cfg[256];
	char hack_on_bin[256];
	char hack_off_bin[256];
	
	char bin_dir[256];
	strcpy(bin_dir, home_dir);
	strcat(bin_dir, "/.local/bin");
	
	strcpy(hack_on_bin, bin_dir);
	strcat(hack_on_bin, "/." APPNAME "-on.sh");
	
	strcpy(hack_off_bin, bin_dir);
	strcat(hack_off_bin, "/." APPNAME "-off.sh");
	
	strcpy(hack_key_cfg, home_dir);
	strcat(hack_key_cfg, "/." APPNAME "-key");
	
	Display *dpy = XOpenDisplay(NULL);
	Window wnd = DefaultRootWindow(dpy);
	
	const int hack_key_num = 133; // TODO read key num from cfg file
	
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
				
				system(hack_on_bin);
				printf("On\n");
				
			} else {
				
				system(hack_off_bin);
				printf("Off\n");
			}
		}
		
		usleep(30000); // idle
	}
	
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
