#ifndef MAINHEADER_H
# define MAINHEADER_H

# include "messages.h"
# include "threadheader.h"
# include "wifi_scan_api.h"
# include <bluetooth/bluetooth.h>
# include <bluetooth/hci.h>
# include <bluetooth/hci_lib.h>
# include <curl/curl.h>
# include <errno.h>
# include <signal.h>
# include <stdatomic.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/ioctl.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <unistd.h>

# define BT_URL "http://192.168.8.109:8000/bluetooth"
# define WIFI_URL "http://192.168.8.109:8000/wifi"

# define IS_TERMINATED() atomic_load(&g_terminate_flag)
# define SET_TERMINATE_FLAG() atomic_store(&g_terminate_flag, true)
# define MAX_HCI_DEVICES 16
# define JOB_MASK_SEND_DATA (1 << JOB_SEND_DATA)
# define JOB_MASK_SCAN_LE_DATA (1 << JOB_SCAN_LE_DATA)
# define JOB_MASK_SCAN_CLASSIC_DATA (1 << JOB_SCAN_CLASSIC_DATA)
# define L2CAP_CID_SIGNALING 0x0001
# define L2CAP_CID_CONNECTIONLESS 0x0002
# define L2CAP_CID_AMP_MANAGER 0x0003
# define L2CAP_CID_LE_ATTRIBUTE 0x0004
# define L2CAP_CID_LE_SIGNALING 0x0005
# define L2CAP_CID_SECURITY_MANAGER 0x0006

typedef enum e_structtype
{
	LE_INFO,
	HCI_INFO,
	CL_INFO,
	ALL_INFO,
	WIFI_INFO,
}								t_structype;

typedef enum
{
	NO_JOB,
	JOB_SEND_DATA,
	JOB_SCAN_LE_DATA,
	JOB_SCAN_CLASSIC_DATA
}								t_hci_dev_job;

typedef struct s_le_scan_dev_info
{
	char						mac_addr[19];
	char						name[100];
	bdaddr_t					bdaddr;
	long long					last_seen_time_ms;
	int8_t						rssi;
	struct s_le_scan_dev_info	*next;
}								t_le_scan_dev_info;

typedef struct s_wifi_dev_info
{
	char						mac_addr[19];
	char						ssid[33];
	long						seen_ms_ago;
	int8_t						rssi;
	float						frequency;
	bool						is_connected;
	struct s_wifi_dev_info		*next;
}								t_wifi_dev_info;

typedef struct s_hci_dev_data
{
	int							dev_id;
	int							sock_fd;
	bdaddr_t					bdaddr;
	char						mac_addr[19];
	char						local_name[248];
	bool						in_use_job;
	uint16_t					hci_handle;
	t_hci_dev_job				job_mask;
	struct s_hci_dev_data		*next;
}								t_hci_dev_data;

typedef struct s_state
{
	t_le_scan_dev_info			*le_scanned_devices;
	t_wifi_dev_info				*wifi_scanned_devices;
	char						wifi_iface[16];
	int							wifi_num_devices;
	bool						wifi_data_updated;
	bool						wifi_scan_on;
	int							bt_num_devices;
	int							bt_dev_fd;
	int							bt_dev_id;
	int							blestatus;
	pthread_mutex_t				thread_error_mutex;
	pthread_mutex_t				ble_data_mutex;
	pthread_mutex_t				wifi_data_mutex;
	volatile int				terminate_flag;
}								t_state;

extern _Atomic bool g_terminate_flag;

// data structures
int								wifi_add_device_lst(t_state *ctx,
									struct bss_info *bss);
int								le_add_scanned_dev_to_lst(t_state *ctx,
									const char *mac_addr, int8_t rssi);
void							remove_device_lst(t_state *ctx,
									void *device_to_remove, t_structype type);
void							free_devices_lst(t_state *ctx,
									t_structype type);

// thread utils
void							pthreads_set_terminate_flag(t_state *ctx);
int								pthreads_check_terminate_flag(t_state *ctx);
long long						timeval_to_ms(void);
// wifi utils
int								get_active_network_interface(char *buffer,
									size_t buffer_size);
const char						*bssid_to_string(const uint8_t bssid[BSSID_LENGTH],
									char bssid_string[BSSID_STRING_LENGTH]);

#endif