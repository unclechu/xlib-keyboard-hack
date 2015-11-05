/**
 * Author: Viacheslav Lotsmanov
 * License: GNU/GPLv3 https://raw.githubusercontent.com/unclechu/xlib-keyboard-hack/master/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <X11/Xlib.h>

#define  APPNAME               "xlib-keyboard-hack"
#define  CFG_ON_KEY_FILENAME   "." APPNAME "-on-key"
#define  CFG_OFF_KEY_FILENAME  "." APPNAME "-off-key"
#define  BIN_ON_FILENAME       "." APPNAME "-on.sh"
#define  BIN_OFF_FILENAME      "." APPNAME "-off.sh"

#define  SLEEP_TIME            30000

int read_cfg_key_num(char *path)
{
	char key_num_str[16] = {'\0'};
	FILE *fp;
	
	fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(stderr, "Can't read file '%s'\n", path);
		exit(EXIT_FAILURE);
	}
	
	for (int i=0; i<sizeof(key_num_str); ) {
		
		char ch = fgetc(fp);
		
		if (ch == EOF || i == sizeof(key_num_str) - 1) {
			key_num_str[i++] = '\0';
			break;
		}
		
		switch (ch) {
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				key_num_str[i++] = ch;
				break;
		}
	}
	
	fclose(fp);
	
	if (strlen(key_num_str) <= 0) {
		fprintf(stderr, "Can't parse key number from file '%s'\n", path);
		exit(EXIT_FAILURE);
	}
	
	int v;
	if ( ! sscanf(key_num_str, "%d", &v)) {
		fprintf(stderr, "Can't parse key number by string from file '%s'\n", path);
		exit(EXIT_FAILURE);
	}
	
	return v;
}

int main(int argc, char **argv)
{
	const char *home_dir = getenv("HOME");
	
	// TODO support on/off mode by different keys
	
	// paths to files
	// TODO cache scripts contents to prevent reading from fs every times
	char hack_on_key_cfg_path[256];
	char hack_off_key_cfg_path[256];
	char hack_on_bin[256];
	char hack_off_bin[256];
	
	char bin_dir[256];
	strcpy(bin_dir, home_dir);
	strcat(bin_dir, "/.local/bin");
	
	// ON bin
	strcpy(hack_on_bin, bin_dir);
	strcat(hack_on_bin, "/" BIN_ON_FILENAME);
	
	// OFF bin
	strcpy(hack_off_bin, bin_dir);
	strcat(hack_off_bin, "/" BIN_OFF_FILENAME);
	
	// ON key
	strcpy(hack_on_key_cfg_path, home_dir);
	strcat(hack_on_key_cfg_path, "/" CFG_ON_KEY_FILENAME);
	// OFF key
	strcpy(hack_off_key_cfg_path, home_dir);
	strcat(hack_off_key_cfg_path, "/" CFG_OFF_KEY_FILENAME);
	
	const int key_on_num = read_cfg_key_num(hack_on_key_cfg_path);
	
	Display *dpy = XOpenDisplay(NULL);
	Window wnd = DefaultRootWindow(dpy);
	
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
						if (key_num == key_on_num) {
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
		
		usleep(SLEEP_TIME); // idle
	}
	
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
