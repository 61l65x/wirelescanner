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

//ERROR MSGS
#define NTWRK_IFACE_ERR_MSG "Failed to get active network interface continue with bluetooth only\n"
#define PTHREAD_ERR_MSG "Failed to initialize pthreads\n"

// ERROR MSGS
# define PTHREAD_ERR_MSG "Failed to initialize pthreads\n"
# define ALLOC_ERR_MSG "Failed to allocate memory\n"
# define UNKWN_ERR_MSG "Unknown error\n"

// HostControlInterface Errors
# define HCI_DEVINFO_ERR_MSG "Failed to get HCI device info\n"
# define BT_HCI_ERR_MSG \
	"Did not find any Bluetooth adapter\n Please make sure your \
Bluetooth adapter is plugged in.\n\
Or the adapter is not compatible with linux systems\n"
# define RFKILL_ERR_MSG "Bluetooth is RF-killed. Attempting to unblock...\n"
# define RFKILL_UNBLOCK_ERR_MSG "Failed to unblock Bluetooth via rfkill.\n"
# define RFKILL_UNBLOCK_SUCCESS_MSG "Bluetooth unblocked successfully. Please wait...\n"
# define BT_UP_ALRDY "Failed to turn on the HCI device"

typedef enum e_retcode
{
	OK,
	FAIL = -1,
}									t_retcode;

#endif
