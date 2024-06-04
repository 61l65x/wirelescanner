
#ifndef NTWRK_HEADER_H
# define NTWRK_HEADER_H

# include <stdbool.h>
# include <stdint.h>

typedef struct s_ntwrk_iface
{
	char						iface_name[50];
	int							ifindex;
	char						wdev[20];
	char						addr[18];
	char						ssid[50];
	char						type[20];
	struct s_ntwrk_iface		*next;
}								t_ntwrk_iface;

typedef struct s_wifi_scan_dev_info
{
	char						mac_addr[19];
	char						ssid[33];
	long						seen_ms_ago;
	int8_t						rssi;
	float						frequency;
	bool						is_connected;
	struct s_wifi_scan_dev_info	*next;
}								t_wifi_scan_dev_info;

typedef struct s_all_ntwrk_info
{
	t_wifi_scan_dev_info		*wifi_scanned_devices;
	t_ntwrk_iface				*ntwrk_ifaces;
	int							num_ntwrk_ifaces;
	int							wifi_num_scanned_devices;
	bool						wifi_data_updated;
	bool						wifi_scan_on;
}								t_all_ntwrk_info;

#endif
