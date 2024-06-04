
#ifndef BT_HEADER_H
# define BT_HEADER_H

# include "messages.h"
# include <bluetooth/bluetooth.h>
# include <bluetooth/hci.h>
# include <bluetooth/hci_lib.h>
# include <errno.h>
# include <stdbool.h>
# include <sys/ioctl.h>

# define HCI_DEVINFO_ERR_MSG "Failed to get HCI device info\n"
# define BT_HCI_ERR_MSG \
	"Did not find any Bluetooth adapter\n Please make sure your \
Bluetooth adapter is plugged in.\n\
Or the adapter is not compatible with linux systems\n"
# define RFKILL_ERR_MSG "Bluetooth is RF-killed. Attempting to unblock...\n"
# define RFKILL_UNBLOCK_ERR_MSG "Failed to unblock Bluetooth via rfkill.\n"
# define RFKILL_UNBLOCK_SUCCESS_MSG "Bluetooth unblocked successfully. Please wait...\n"
# define BT_UP_ALRDY "Failed to turn on the HCI device"
# define ERR_CALLOC_HCI "Failed to allocate memory for HCI device\n"
# define MAX_HCI_DEVICES 16
# define JOB_MASK_SEND_DATA (1 << HCI_JOB_SEND_DATA)
# define JOB_MASK_SCAN_LE_DATA (1 << HCI_JOB_SCAN_LE_DATA)
# define JOB_MASK_SCAN_CLASSIC_DATA (1 << HCI_JOB_SCAN_CLASSIC_DATA)
# define L2CAP_CID_SIGNALING 0x0001
# define L2CAP_CID_CONNECTIONLESS 0x0002
# define L2CAP_CID_AMP_MANAGER 0x0003
# define L2CAP_CID_LE_ATTRIBUTE 0x0004
# define L2CAP_CID_LE_SIGNALING 0x0005
# define L2CAP_CID_SECURITY_MANAGER 0x0006

typedef struct s_state				t_state;

typedef enum e_iface_job
{
	HCI_NO_JOB,
	HCI_JOB_SEND_DATA,
	HCI_JOB_SCAN_LE_DATA,
	HCI_JOB_SCAN_CLASSIC_DATA,
	NTWRK_NO_JOB,
	NTWRK_JOB_SEND_DATA,
	NTWRK_JOB_SCAN_DATA,
}									t_iface_job;

typedef struct s_le_scan_dev_info
{
	char							mac_addr[19];
	char							name[100];
	bdaddr_t						bdaddr;
	long long						last_seen_time_ms;
	int8_t							rssi;
	struct s_le_scan_dev_info		*next;
}									t_le_scan_dev_info;

typedef struct s_cl_inquiry_dev_info
{
	char							mac_addr[19];
	bdaddr_t						bdaddr;
	char							name[248];
	uint8_t							cod[3];
	long long						last_seen_time_ms;
	int								rssi;
	struct s_cl_inquiry_dev_info	*next;
}									t_cl_inquiry_dev_info;

typedef struct s_bt_hci_iface
{
	int								dev_id;
	int								sock_fd;
	bdaddr_t						bdaddr;
	char							mac_addr[19];
	char							local_name[248];
	bool							in_use_job;
	uint16_t						hci_handle;
	t_iface_job						job_mask;
	struct s_bt_hci_iface			*next;
}									t_bt_hci_iface;

typedef struct s_all_bt_info
{
	t_le_scan_dev_info				*le_scanned_devices;
	t_cl_inquiry_dev_info			*cl_scanned_devices;
	t_bt_hci_iface					*hci_ifaces;
	int								cl_num_scanned_devices;
	int								num_hci_devices;
	int								le_num_scanned_devices;
	pthread_mutex_t					le_data_mutex;
	pthread_mutex_t					hci_data_mutex;
	pthread_mutex_t					cl_data_mutex;
}									t_all_bt_info;

#endif
