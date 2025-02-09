
#include "wirelescanner.h"
#include "wifi_scan_api.h"
#define MAX_BSS_ENTRIES 100

const char	*bssid_to_string(const uint8_t bssid[BSSID_LENGTH],
		char bssid_string[BSSID_STRING_LENGTH])
{
	snprintf(bssid_string, BSSID_STRING_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x",
		bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
	return (bssid_string);
}

static int	wifi_add_device_lst(t_state *ctx, struct bss_info *bss)
{
	t_wifi_scan_dev_info	*new_device;

	new_device = calloc(1, sizeof(t_wifi_scan_dev_info));
	if (!new_device)
		return (perror("malloc in add_wifi_device"), -1);
	bssid_to_string(bss->bssid, new_device->mac_addr);
	snprintf(new_device->ssid, sizeof(new_device->ssid), "%s", bss->ssid);
	new_device->seen_ms_ago = bss->seen_ms_ago;
	new_device->rssi = bss->signal_mbm / 100;
	// Assuming signal_mbm is in millibels and you want it in dB
	new_device->frequency = (float)bss->frequency / 1000.0;
	new_device->is_connected = bss->status == BSS_ASSOCIATED ? 1 : 0;
	new_device->next = ctx->ntwrk_info.wifi_scanned_devices;
	ctx->ntwrk_info.wifi_scanned_devices = new_device;
	return (0);
}

static void	wifi_update_device(t_wifi_scan_dev_info *current,
		struct bss_info *bss)
{
	snprintf(current->ssid, sizeof(current->ssid), "%s", bss->ssid);
	current->seen_ms_ago = bss->seen_ms_ago;
	current->rssi = bss->signal_mbm / 100;
	// Assuming signal_mbm is in millibels and you want it in dB
	current->frequency = (float)bss->frequency / 1000.0;
	current->is_connected = bss->status == BSS_ASSOCIATED ? 1 : 0;
}

static int	add_update_wifi_device(t_state *ctx, struct bss_info *bss)
{
	char					bssid_str[BSSID_STRING_LENGTH];
	t_wifi_scan_dev_info	*current;

	current = ctx->ntwrk_info.wifi_scanned_devices;
	bssid_to_string(bss->bssid, bssid_str);
	while (current != NULL)
	{
		if (strcmp(current->mac_addr, bssid_str) == 0)
			return (wifi_update_device(current, bss), 0);
		current = current->next;
	}
	return (wifi_add_device_lst(ctx, bss));
}

void	*wifi_scan_thread(void *arg)
{
	t_state				*ctx;
	struct wifi_scan	*wifi;
	struct bss_info		bss[MAX_BSS_ENTRIES];
	char				bssid_str[BSSID_STRING_LENGTH];

	ctx = (t_state *)arg;
	wifi = NULL;
	int scan_count, i;
	printf("wifi iface name is %s\n", ctx->ntwrk_info.ntwrk_ifaces->iface_name);
	if (ctx == NULL || !ctx->ntwrk_info.ntwrk_ifaces)
		return (perror("wifi_iface is not initialized"), NULL);
	if ((wifi = wifi_scan_init(ctx->ntwrk_info.ntwrk_ifaces->iface_name)) == NULL)
		return (perror("Failed to initialize wifi scan"), NULL);
	while (!IS_TERMINATED())
	{
		if ((scan_count = wifi_scan_all(wifi, bss, MAX_BSS_ENTRIES)) < 0)
			perror("Unable to get scan data");
		else
		{
			pthread_mutex_lock(&ctx->wifi_data_mutex);
			for (i = 0; i < scan_count && i < MAX_BSS_ENTRIES; ++i)
			{
				if (add_update_wifi_device(ctx, &bss[i]) < 0)
					SET_TERMINATE_FLAG();
				printf("SSID: %s, BSSID: %s, RSSI: %d dBm, Frequency: %.3f GHz,\
					Last seen: %d ms ago\n",
						bss[i].ssid,
						bssid_to_string(bss[i].bssid, bssid_str),
						bss[i].signal_mbm / 100,
						(float)bss[i].frequency / 1000.0,
						bss[i].seen_ms_ago);
			}
			ctx->ntwrk_info.wifi_data_updated = true;
			pthread_mutex_unlock(&ctx->wifi_data_mutex);
		}
		usleep(10);
	}
	wifi_scan_close(wifi);
	return (NULL);
}
