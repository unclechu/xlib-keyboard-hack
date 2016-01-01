/**
 * Author: Viacheslav Lotsmanov
 * License: GNU/GPLv3 https://raw.githubusercontent.com/unclechu/xlib-keyboard-hack/master/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#define  APPNAME               "xlib-keyboard-hack"
#define  CFG_ON_KEY_FILENAME   "." APPNAME "-on-key"
#define  CFG_OFF_KEY_FILENAME  "." APPNAME "-off-key"
#define  BIN_ON_FILENAME       "." APPNAME "-on.sh"
#define  BIN_OFF_FILENAME      "." APPNAME "-off.sh"

#define  SLEEP_TIME            30000
#define  FILE_SIZE             10000

#define  SHELL                 "/bin/bash"

#define  KEYS_LIMIT            16

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

void show_help()
{
	const paths my_paths = get_paths();
	
	printf(
		"Usage\n"
		"=====\n"
		"  -h --help\n"
		"    Shows this info.\n"
		"  -t --toggle\n"
		"    Toggle switch mode by two keys (first switching on, second switching off)\n"
		"  -m --modifier\n"
		"    Modifier key mode, when pressed then switching on, and switching off when released.\n"
		"-----\n"
		"Create this bash scripts with on/off actions:\n"
		"  On: %s\n"
		"  Off: %s\n"
		"-----\n"
		"Create this files with numbers of keys to on/off:\n"
		"  On: %s\n"
		"  Off (only for --toggle mode): %s\n",
		my_paths.on_bin, my_paths.off_bin,
		my_paths.on_key_cfg, my_paths.off_key_cfg
	);
}

typedef enum { TOGGLE, MODIFIER } app_mode;

void trig_off_keys(
	Display *dpy,
	const int *stored_keys_count,
	const int *stored_keys
) {
	for (int i=0; i<(*stored_keys_count); i++) {
		printf("Trigger key-off for keycode: %d\n", stored_keys[i]);
		XTestFakeKeyEvent(dpy, stored_keys[i], False, 0);
		XFlush(dpy);
	}
}

int main(const int argc, const char **argv)
{
	if (argc > 2) {
		fprintf(stderr, "Unexpected arguments count.\n");
		return EXIT_FAILURE;
	}
	
	if (argc != 2) {
		show_help();
		return EXIT_FAILURE;
	}
	
	app_mode mode;
	
	if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
		show_help();
		return EXIT_SUCCESS;
	} else if (strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--toggle") == 0) {
		printf("Started with toggle switching mode.\n");
		mode = TOGGLE;
	} else if (strcmp(argv[1], "-m") == 0 || strcmp(argv[1], "--modifier") == 0) {
		printf("Started with modifier mode.\n");
		mode = MODIFIER;
	} else {
		fprintf(stderr, "Unrecognized argument: '%s'.\n", argv[1]);
		show_help();
		return EXIT_FAILURE;
	}
	
	const paths my_paths = get_paths();
	
	const int key_on_num  = read_cfg_key_num(my_paths.on_key_cfg);
	const int key_off_num =
		(mode == TOGGLE) ? read_cfg_key_num(my_paths.off_key_cfg) : -1;
	
	printf("On-key number: %d\n", key_on_num);
	if (mode == TOGGLE) {
		printf("Off-key number: %d\n", key_off_num);
	}
	
	const char* on_bin_cache  = read_file_to_str(my_paths.on_bin);
	const char* off_bin_cache = read_file_to_str(my_paths.off_bin);
	
	Display *dpy = XOpenDisplay(NULL);
	Window   wnd = DefaultRootWindow(dpy);
	
	int  last_key_state = 0; // when is --toggle mode and on/off keys is same
	int  last_state     = 0;
	char keys_return[KEYS_LIMIT];
	while (1) {
		
		XQueryKeymap(dpy, keys_return);
		
		int on_key_state  = 0;
		int off_key_state = 0;
		
		// for triggering keyoffs
		int stored_keys[KEYS_LIMIT];
		int stored_keys_count = 0;
		
		int is_off_triggered = 0;
		
		for (int i=0; i<KEYS_LIMIT; i++) {
			
			if (keys_return[i] != 0) {
				
				int pos = 0;
				int num = keys_return[i];
				
				while (pos < 8) {
					
					if ((num & 0x01) == 1) {
						
						int key_num = i*8+pos;
						
						if (key_num == key_on_num) {
							on_key_state = 1;
						}
						if (mode == TOGGLE && key_num == key_off_num) {
							off_key_state = 1;
						}
						
						if (key_num != key_on_num && key_num != key_off_num) {
							stored_keys[stored_keys_count++] = key_num;
						}
					}
					
					++pos;
					num /= 2;
				}
			}
		}
		
		if (mode == TOGGLE) {
			
			// if on and off keys is same key
			if (key_on_num == key_off_num) {
				if (last_key_state == 1 && on_key_state == 0) {
					last_key_state = 0;
					on_key_state   = 0;
					off_key_state  = 0;
				} else if (last_key_state == 0 && on_key_state == 1) {
					last_key_state = 1;
					on_key_state   = 1;
					off_key_state  = 1;
				} else {
					on_key_state   = 0;
					off_key_state  = 0;
				}
			}
			
			if (last_state == 0) {
				if (on_key_state == 1) {
					last_state = 1;
					run_script(on_bin_cache);
					printf("On\n");
				}
			} else {
				if (off_key_state == 1) {
					last_state = 0;
					trig_off_keys(dpy, &stored_keys_count, stored_keys);
					run_script(off_bin_cache);
					printf("Off\n");
				}
			}
			
		} else if (mode == MODIFIER) {
			
			if (last_state != on_key_state) {
				last_state = on_key_state;
				if (last_state == 1) {
					run_script(on_bin_cache);
					printf("On\n");
				} else {
					trig_off_keys(dpy, &stored_keys_count, stored_keys);
					run_script(off_bin_cache);
					printf("Off\n");
				}
			}
			
		} else {
			fprintf(stderr, "Unknown mode\n");
			return EXIT_FAILURE;
		}
		
		usleep(SLEEP_TIME); // idle
	}
	
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
