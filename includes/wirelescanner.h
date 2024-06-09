#ifndef WIRELESCANNER_H
# define WIRELESCANNER_H

# include "bt_header.h"
# include "messages.h"
# include "ntwrk_header.h"
# include "threadheader.h"
# include "wifi_scan_api.h"
# include <bits/pthreadtypes.h>
# include <bluetooth/bluetooth.h>
# include <curl/curl.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <stdatomic.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <unistd.h>
#include "ble_parser.h"

# define IS_TERMINATED() atomic_load(&g_terminate_flag)
# define SET_TERMINATE_FLAG() atomic_store(&g_terminate_flag, true)

extern _Atomic bool g_terminate_flag;

typedef enum e_structtype
{
	LE_INFO,
	HCI_INFO,
	NTWRK_INFO,
	CL_INFO,
	ALL_INFO,
	WIFI_INFO,
}						t_structype;

typedef struct s_state
{
	t_all_ntwrk_info	ntwrk_info;
	t_all_bt_info		bt_info;
	pthread_mutex_t		wifi_data_mutex;
}						t_state;

void					init_signals(t_state *state);
int						le_add_scanned_dev_to_lst(t_state *s,
							const bdaddr_t *bdaddr, const char *mac_addr,
							int8_t rssi);
void					remove_from_lst(t_state *ctx, void *device_to_remove,
							t_structype type);
void					clear_lst(t_state *ctx, t_structype type);
void					le_update_add_dev(t_state *s, const bdaddr_t *bdaddr,
							int8_t rssi);
long long				timeval_to_ms(void);
int						get_active_network_interface(char *buffer,
							size_t buffer_size);
const char				*bssid_to_string(const uint8_t bssid[BSSID_LENGTH],
							char bssid_string[BSSID_STRING_LENGTH]);
void					eir_parse_name(uint8_t *eir, size_t eir_len, char *buf,
							size_t buf_len);
t_bt_hci_iface			*get_hci_for_job(t_all_bt_info *s, t_iface_job job);
void					free_exit(const char *err_msg, t_state *state);
#endif
