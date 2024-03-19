#ifndef MESSAGES_H
#define MESSAGES_H

#define C_RED     "\x1b[31m"
#define C_GREEN   "\x1b[32m"
#define C_YELLOW  "\x1b[33m"
#define C_BLUE    "\x1b[34m"
#define C_MAGENTA "\x1b[35m"
#define C_CYAN    "\x1b[36m"
#define C_RESET   "\x1b[0m"

#define ARG_OPTIONS \
    "Usage: wifi - to scan for wifi devices also\n" \
    "\tsudo " C_GREEN "./scanner" C_RESET " " C_YELLOW "[wifi]" C_RESET "\n"

# define BT_HCI_ERR_MSG "Failed to initialize Bluetooth HCI\n"
//ERROR MSGS
#define NTWRK_IFACE_ERR_MSG "Failed to get active network interface continue with bluetooth only\n"
#define PTHREAD_ERR_MSG "Failed to initialize pthreads\n"

#endif
