
#include "mainheader.h"
#include "wifi_scan_api.h"
#define MAX_BSS_ENTRIES 100

static int	add_update_wifi_device(t_state *ctx, struct bss_info *bss)
{
	char			bssid_str[BSSID_STRING_LENGTH];
	t_wifi_dev_info	*current;

	current = ctx->wifi_scanned_devices;
	bssid_to_string(bss->bssid, bssid_str);
	while (current != NULL)
	{
		if (strcmp(current->mac_addr, bssid_str) == 0)
		{
			snprintf(current->ssid, sizeof(current->ssid), "%s", bss->ssid);
			current->seen_ms_ago = bss->seen_ms_ago;
			current->rssi = bss->signal_mbm / 100;
			// Assuming signal_mbm is in millibels and you want it in dB
			current->frequency = (float)bss->frequency / 1000.0;
			current->is_connected = bss->status == BSS_ASSOCIATED ? 1 : 0;
			return (0);
		}
		current = current->next;
	}
	return (wifi_add_device_lst(ctx, bss));
}

void	*wifi_scan_thread(void *arg)
{
	t_state				*ctx;
	struct wifi_scan	*wifi;
	struct bss_info		bss[MAX_BSS_ENTRIES];

	ctx = (t_state *)arg;
	wifi = NULL;
	int scan_count, i;
	if (ctx == NULL || ctx->wifi_iface[0] == '\0')
		return (perror("wifi_iface is not initialized"), NULL);
	if ((wifi = wifi_scan_init(ctx->wifi_iface)) == NULL)
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
			}
			ctx->wifi_data_updated = true;
			pthread_mutex_unlock(&ctx->wifi_data_mutex);
		}
		usleep(10);
	}
	wifi_scan_close(wifi);
	return (NULL);
}
