#ifndef MAINHEADER_H
# define MAINHEADER_H

# include "messages.h"
# include "threadheader.h"
# include "wifi_scan_api.h"
# include <bits/pthreadtypes.h>
# include <bluetooth/bluetooth.h>
# include <bluetooth/hci.h>
# include <bluetooth/hci_lib.h>
# include <curl/curl.h>
# include <errno.h>
# include <fcntl.h>
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
# define JOB_MASK_SEND_DATA (1 << HCI_JOB_SEND_DATA)
# define JOB_MASK_SCAN_LE_DATA (1 << HCI_JOB_SCAN_LE_DATA)
# define JOB_MASK_SCAN_CLASSIC_DATA (1 << HCI_JOB_SCAN_CLASSIC_DATA)
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
	NTWRK_INFO,
	CL_INFO,
	ALL_INFO,
	WIFI_INFO,
}									t_structype;

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

typedef struct s_wifi_scan_dev_info
{
	char							mac_addr[19];
	char							ssid[33];
	long							seen_ms_ago;
	int8_t							rssi;
	float							frequency;
	bool							is_connected;
	struct s_wifi_scan_dev_info		*next;
}									t_wifi_scan_dev_info;

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

typedef struct s_ntwrk_iface
{
	char							iface_name[50];
	int								ifindex;
	char							wdev[20];
	char							addr[18];
	char							ssid[50];
	char							type[20];
	struct s_ntwrk_iface			*next;
}									t_ntwrk_iface;

typedef struct s_state
{
	t_le_scan_dev_info				*le_scanned_devices;
	t_wifi_scan_dev_info			*wifi_scanned_devices;
	t_cl_inquiry_dev_info			*cl_scanned_devices;
	t_bt_hci_iface					*hci_ifaces;
	int								cl_num_scanned_devices;
	int								num_hci_devices;
	int								wifi_num_scanned_devices;
	bool							wifi_data_updated;
	char							wifi_iface_name[16];
	bool							wifi_scan_on;
	int								le_num_scanned_devices;
	pthread_mutex_t					le_data_mutex;
	pthread_mutex_t					wifi_data_mutex;
	pthread_mutex_t					hci_data_mutex;
	pthread_mutex_t					cl_data_mutex;
}									t_state;

extern _Atomic bool g_terminate_flag;

int									init_bluetooth_ifaces(t_state *s);
void								init_signals(t_state *state);
// data structures
int									le_add_scanned_dev_to_lst(t_state *s,
										const bdaddr_t *bdaddr,
										const char *mac_addr, int8_t rssi);
void								remove_dev_from_lst(t_state *ctx,
										void *device_to_remove,
										t_structype type);
void								clear_lst(t_state *ctx, t_structype type);
void								le_update_add_dev(t_state *s,
										const bdaddr_t *bdaddr, int8_t rssi);

// thread utils
long long							timeval_to_ms(void);
// wifi utils
int									get_active_network_interface(char *buffer,
										size_t buffer_size);
const char							*bssid_to_string(const uint8_t bssid[BSSID_LENGTH],
										char bssid_string[BSSID_STRING_LENGTH]);
void								eir_parse_name(uint8_t *eir, size_t eir_len,
										char *buf, size_t buf_len);
t_bt_hci_iface						*get_hci_for_job(t_state *s,
										t_iface_job job);

#endif