NAME = wirelescanner
PATH_LIBBLEPARSE = dependencies/libbleparser

BUILD_DIR := build

LD_BLEPARSE_PATH = -L./$(PATH_LIBBLEPARSE)/build -lbleparser
CFLAGS = -Wall -g -O2 -MMD -MP
LIBS = `pkg-config --cflags libcurl --libs libcurl bluez` -lbluetooth -lpthread -lmnl -lyaml $(LD_BLEPARSE_PATH)
INCLUDES = -I./includes -I./dependencies/wifiscan -I$(PATH_LIBBLEPARSE)/includes

SRCS := $(shell find srcs -name "*.c" && find dependencies/wifiscan -name "*.c")
OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))
DEPS := $(patsubst %.c, $(BUILD_DIR)/%.d, $(SRCS))

# Create build directories
$(shell mkdir -p $(sort $(dir $(OBJS))))

all: dependencies $(NAME)

$(BUILD_DIR)/%.o: %.c
	@echo "\033[0;34mCompiling... $<\033[0m"
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "\033[0;32mDone. Run 'make help' for all rules! 📡🌐\033[0m"

dependencies:
	@echo "\033[0;32mBuilding libbleparser...\033[0m"
	@make -C $(PATH_LIBBLEPARSE) || (echo "\033[0;31mBuild failed. Attempting to install dependencies...\033[0m" && make install-deps && make -C $(PATH_LIBBLEPARSE))
	git pull origin main

test:
	$(CC) $(CFLAGS) -g $(INCLUDES) -o test tests/test.c $(LD_BLEPARSE_PATH) -lyaml

clean:
	@echo "\033[0;31mCleaning...\033[0m"
	@rm -rf $(BUILD_DIR)
	@make -C $(PATH_LIBBLEPARSE) clean

fclean:
	@echo "\033[0;31mFull Cleaning... $(NAME)\033[0m"
	@rm -f $(NAME)
	@rm -rf $(BUILD_DIR)
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

-include $(DEPS)
