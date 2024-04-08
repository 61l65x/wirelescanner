NAME=btscanner
CFLAGS = -Wall -g -O2 
LIBS = `pkg-config --cflags libcurl --libs libcurl bluez` -lbluetooth -lpthread -lmnl
INCLUDES = -I./includes -I./apis/wifiscan
SRCS = $(shell find srcs -name "*.c" && find apis -name "*.c")
OBJS = $(SRCS:.c=.o)

all: $(NAME)

%.o: %.c
	@echo "\033[0;34mCompiling... $<\033[0m"
	@gcc $(CFLAGS) -c -o $@ $< $(INCLUDES)

$(NAME): $(OBJS)
	@gcc $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "\033[0;32mDone Run 'make help' for all rules! 📡🌐\033[0m"

clean:
	@echo "\033[0;31mCleaning...\033[0m"
	@rm -f $(NAME) $(OBJS)

fclean:
	@echo "\033[0;31m F Cleaning...\033[0m"
	@rm -f $(NAME) $(OBJS)

re: clean all

install-deps:
	@echo "\033[0;32mInstalling dependencies...\033[0m"
	sudo apt-get update
	sudo apt-get install -y libcurl4-openssl-dev libbluetooth-dev libmnl-dev

help:
	@echo "\033[0;33mAvailable rules:\033[0m"
	@echo "\033[0;33m  all: Build the project\033[0m"
	@echo "\033[0;33m  clean: Remove build artifacts\033[0m"
	@echo "\033[0;33m  re: Rebuild the project\033[0m"
	@echo "\033[0;33m  install-deps: Install required dependencies\033[0m"

.PHONY: all clean re install-deps help install-deps help
