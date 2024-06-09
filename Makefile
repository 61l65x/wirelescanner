NAME = wirelescanner
PATH_LIBBLEPARSE = dependencies/libbleparser
#PATH_LIBYAML = dependencies/libyaml

LD_BLEPARSE_PATH = -L$(PATH_LIBBLEPARSE)/build -lbleparser
CFLAGS = -Wall -g -O2
LIBS = `pkg-config --cflags libcurl --libs libcurl bluez` -lbluetooth -lpthread -lmnl -lyaml $(LD_BLEPARSE_PATH)
INCLUDES = -I./includes -I./dependencies/wifiscan -I$(PATH_LIBBLEPARSE)/includes
SRCS = $(shell find srcs -name "*.c" && find dependencies/wifiscan -name "*.c")
OBJS = $(SRCS:.c=.o)

all: dependencies $(NAME)

%.o: %.c
	@echo "\033[0;34mCompiling... $<\033[0m"
	@gcc $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(NAME): $(OBJS)
	@gcc $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "\033[0;32mDone. Run 'make help' for all rules! 📡🌐\033[0m"

dependencies: libbleparser

libbleparser:
	@echo "\033[0;32mBuilding libbleparser...\033[0m"
	@make -C $(PATH_LIBBLEPARSE)

clean:
	@echo "\033[0;31mCleaning...\033[0m"
	@rm -f $(NAME) $(OBJS)
	@make -C $(PATH_LIBBLEPARSE) clean

fclean:
	@echo "\033[0;31mFull Cleaning...\033[0m"
	@rm -f $(NAME) $(OBJS)
	@make -C $(PATH_LIBBLEPARSE) fclean

re: clean all

install-deps:
	@echo "\033[0;32mInstalling dependencies...\033[0m"
	sudo apt-get update
	sudo apt-get install -y libcurl4-openssl-dev libbluetooth-dev libmnl-dev iw libyaml-dev

help:
	@echo "\033[0;33mAvailable rules:\033[0m"
	@echo "\033[0;33m  all: Build the project\033[0m"
	@echo "\033[0;33m  clean: Remove build artifacts\033[0m"
	@echo "\033[0;33m  re: Rebuild the project\033[0m"
	@echo "\033[0;33m  install-deps: Install required dependencies\033[0m"

.PHONY: all clean fclean re install-deps help dependencies libbleparser libyaml
