#include "mainheader.h"

int	wifi_add_device_lst(t_state *ctx, struct bss_info *bss)
{
	t_wifi_dev_info	*new_device;

	new_device = calloc(1, sizeof(t_wifi_dev_info));
	if (!new_device)
	{
		perror("malloc in add_wifi_device");
		return (-1);
	}
	bssid_to_string(bss->bssid, new_device->mac_addr);
	snprintf(new_device->ssid, sizeof(new_device->ssid), "%s", bss->ssid);
	new_device->seen_ms_ago = bss->seen_ms_ago;
	new_device->rssi = bss->signal_mbm / 100;
		// Assuming signal_mbm is in millibels and you want it in dB
	new_device->frequency = (float)bss->frequency / 1000.0;
	new_device->is_connected = bss->status == BSS_ASSOCIATED ? 1 : 0;
	new_device->next = ctx->wifi_scanned_devices;
	ctx->wifi_scanned_devices = new_device;
	return (0);
}

int	le_add_scanned_dev_to_lst(t_state *ctx, const char *mac_addr, int8_t rssi)
{
	t_le_scan_dev_info	*new_device;

	new_device = calloc(1, sizeof(t_le_scan_dev_info));
	if (!new_device)
	{
		perror("malloc in bt_add_device_lst");
		return (-1);
	}
	strcpy(new_device->mac_addr, mac_addr);
	new_device->last_seen_time_ms = timeval_to_ms();
	new_device->rssi = rssi;
	new_device->next = ctx->le_scanned_devices;
	ctx->le_scanned_devices = new_device;
	ctx->bt_num_devices++;
	return (0);
}

void	remove_device_lst(t_state *ctx, void *device_to_remove,
		t_structype type)
{
	t_le_scan_dev_info	*current;
	t_le_scan_dev_info	*previous;

	if (type == LE_INFO)
	{
		current = ctx->le_scanned_devices;
		previous = NULL;
		while (current != NULL)
		{
			if (current == (t_le_scan_dev_info *)device_to_remove)
			{
				if (previous == NULL)
					ctx->le_scanned_devices = current->next;
				else
					previous->next = current->next;
				free(current);
				ctx->bt_num_devices--;
				return ;
			}
			previous = current;
			current = current->next;
		}
	}
	else if (type == WIFI_INFO)
	{
        t_wifi_dev_info	*current;
	    t_wifi_dev_info	*previous;
		current = ctx->wifi_scanned_devices;
		previous = NULL;
		while (current != NULL)
		{
			if (current == (t_wifi_dev_info *)device_to_remove)
			{
				if (previous == NULL)
					ctx->wifi_scanned_devices = current->next;
				else
					previous->next = current->next;
				free(current);
				ctx->wifi_num_devices--;
				return ;
			}
			previous = current;
			current = current->next;
		}
	}
}

void	clear_lst(t_state *ctx, t_structype type)
{
	t_le_scan_dev_info	*current;
		t_le_scan_dev_info *next;

	if (type == LE_INFO)
	{
		current = ctx->le_scanned_devices;
		while (current != NULL)
		{
			next = current->next;
			free(current);
			current = next;
		}
		ctx->le_scanned_devices = NULL;
		ctx->bt_num_devices = 0;
	}
	else if (type == WIFI_INFO)
	{
        t_wifi_dev_info	*current;
		t_wifi_dev_info *next;
		current = ctx->wifi_scanned_devices;
		while (current != NULL)
		{
			next = current->next;
			free(current);
			current = next;
		}
		ctx->wifi_scanned_devices = NULL;
		ctx->bt_num_devices = 0;
	}
}
