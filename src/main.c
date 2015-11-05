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
#define  FILE_SIZE             10000

#define  SHELL                 "/bin/bash"

int read_cfg_key_num(const char *path)
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

typedef struct {
	char on_key_cfg[255];
	char off_key_cfg[255];
	char on_bin[255];
	char off_bin[255];
} paths;

paths get_paths()
{
	paths my_paths;
	const char *home_dir = getenv("HOME");
	
	char bin_dir[256];
	strcpy(bin_dir, home_dir);
	strcat(bin_dir, "/.local/bin");
	
	// ON bin
	strcpy(my_paths.on_bin, bin_dir);
	strcat(my_paths.on_bin, "/" BIN_ON_FILENAME);
	// OFF bin
	strcpy(my_paths.off_bin, bin_dir);
	strcat(my_paths.off_bin, "/" BIN_OFF_FILENAME);
	
	// ON key
	strcpy(my_paths.on_key_cfg, home_dir);
	strcat(my_paths.on_key_cfg, "/" CFG_ON_KEY_FILENAME);
	// OFF key
	strcpy(my_paths.off_key_cfg, home_dir);
	strcat(my_paths.off_key_cfg, "/" CFG_OFF_KEY_FILENAME);
	
	return my_paths;
}

char* read_file_to_str(const char *path)
{
	char *str = (char*)malloc(sizeof(char) * FILE_SIZE);
	FILE *fp;
	
	fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(stderr, "Can't read file '%s'\n", path);
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	
	str[0] = '\0';
	for (int i=0; i<FILE_SIZE; ) {
		
		char ch = fgetc(fp);
		
		if (ch == EOF || i == FILE_SIZE - 1) {
			str[i++] = '\0';
			break;
		}
		
		str[i++] = ch;
	}
	
	fclose(fp);
	return str;
}

void run_script(const char *script_code)
{
	const pid_t child_pid = fork();
	
	if (child_pid < 0) {
		fprintf(stderr, "Can't fork.\n");
		perror("fork");
		exit(EXIT_FAILURE);
	}
	
	// parent process
	if (child_pid != 0) {
		return;
	}
	
	if (execl(SHELL, SHELL, "-c", script_code, NULL) == -1) {
		fprintf(stderr, "Bash-script execution error.\n");
		perror("execl");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv)
{
	
	// TODO support on/off mode by different keys
	
	const paths my_paths = get_paths();
	
	const int key_on_num = read_cfg_key_num(my_paths.on_key_cfg);
	
	const char* on_bin_cache = read_file_to_str(my_paths.on_bin);
	const char* off_bin_cache = read_file_to_str(my_paths.off_bin);
	
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
				
				run_script(on_bin_cache);
				printf("On\n");
				
			} else {
				
				run_script(off_bin_cache);
				printf("Off\n");
			}
		}
		
		usleep(SLEEP_TIME); // idle
	}
	
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
