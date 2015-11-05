NAME ?= xlib-keyboard-hack
BUILD_DIR ?= ./build/

$(NAME): clean
	mkdir "$(BUILD_DIR)"
	gcc \
		./src/main.c \
		-lX11 \
		-o "$(BUILD_DIR)/$(NAME)"

clean:
	rm -rf "$(BUILD_DIR)"

all: $(NAME)
