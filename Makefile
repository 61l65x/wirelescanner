NAME = wirelescanner

BUILD_DIR := build
PATH_LIBBLEPARSE = srcs/dependencies/libbleparser
LD_BLEPARSE_PATH = -L./$(PATH_LIBBLEPARSE)/build -lbleparser

CFLAGS = -Wall -g -O2 -MMD -MP
LIBS = `pkg-config --cflags libcurl --libs libcurl bluez` -lbluetooth -lpthread -lmnl -lyaml $(LD_BLEPARSE_PATH)
INCLUDES = -I./srcs/includes -I./srcs/dependencies/wifiscan -I$(PATH_LIBBLEPARSE)/srcs/includes

SRCS := $(shell find srcs/core -name "*.c") $(shell find srcs/dependencies/wifiscan -name "*.c")
OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))
DEPS := $(patsubst %.c, $(BUILD_DIR)/%.d, $(SRCS))

# LINKING

-include $(DEPS)

$(shell mkdir -p $(sort $(dir $(OBJS))))

$(BUILD_DIR)/%.o: %.c
	@echo "\033[0;34mCompiling... $<\033[0m"
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

# COMPILE PROJECT

all: dependencies $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $^ $(LIBS)
	@echo "\033[0;32mDone. Run 'make help' for all rules! 📡🌐\033[0m"

# DEPENDENCIES

dependencies: check-deps libbleparser-build

check-deps:
	@echo "\033[0;34mChecking system dependencies...\033[0m"
	@if pkg-config --exists libcurl bluez yaml-0.1 mnl; then \
		echo "\033[0;31mMissing dependencies detected. Installing...\033[0m"; \
		sudo apt-get install -y libcurl4-openssl-dev libbluetooth-dev libmnl-dev iw libyaml-dev; \
	else \
		echo "\033[0;32mAll system dependencies are satisfied.\033[0m"; \
	fi

libbleparser-build:
	@echo "\033[0;34mInitializing and building libbleparser...\033[0m"
	@if [ ! -d "$(PATH_LIBBLEPARSE)/.git" ]; then \
		git submodule update --init --recursive; \
	fi
	@if [ ! -f "$(PATH_LIBBLEPARSE)/build/libbleparser.a" ]; then \
		$(MAKE) -C $(PATH_LIBBLEPARSE); \
	else \
		echo "\033[0;32mlibbleparser is already built.\033[0m"; \
	fi




# TESTS

test:
	$(CC) $(CFLAGS) -g $(INCLUDES) -o test srcs/tests/test.c $(LIBS)


# CLEANING

clean:
	@echo "\033[0;31mCleaning...\033[0m"
	@rm -rf $(BUILD_DIR)
	@make -C $(PATH_LIBBLEPARSE) clean

fclean:
	@echo "\033[0;31mFull Cleaning... $(NAME)\033[0m"
	@rm -f $(NAME)
	@rm -rf $(BUILD_DIR)
	@rm -rf $(PATH_LIBBLEPARSE)
#	@make -C $(PATH_LIBBLEPARSE) fclean

re: clean all


.PHONY: all clean fclean re install-deps help dependencies libbleparser libyaml test check-deps libbleparser-build
 
