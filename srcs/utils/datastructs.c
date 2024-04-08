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

void	remove_dev_from_lst(t_state *s, void *dev_to_remove, t_structype type)
{
	if (type == LE_INFO)
	{
		for (t_le_scan_dev_info *current = s->le_scanned_devices,
			*prev = NULL; current != NULL; prev = current,
			current = current->next)
		{
			if (current == (t_le_scan_dev_info *)dev_to_remove)
			{
				if (prev == NULL)
					s->le_scanned_devices = current->next;
				else
					prev->next = current->next;
				s->le_num_scanned_devices--;
				free(current);
				current = NULL;
				return ;
			}
		}
	}
	if (type == HCI_INFO)
	{
		for (t_hci_dev_data *current = s->hci_devices,
			*prev = NULL; current != NULL; prev = current,
			current = current->next)
		{
			if (current == (t_hci_dev_data *)dev_to_remove)
			{
				if (prev == NULL)
					s->hci_devices = current->next;
				else
					prev->next = current->next;
				s->num_hci_devices--;
				if (current->sock_fd >= 0)
					hci_close_dev(current->sock_fd);
				free(current);
				current = NULL;
				return ;
			}
		}
	}
	else if (type == WIFI_INFO)
	{
		for (t_wifi_dev_info *current = s->wifi_scanned_devices,
			*prev = NULL; current != NULL; prev = current,
			current = current->next)
		{
			if (current == (t_wifi_dev_info *)dev_to_remove)
			{
				if (prev == NULL)
					s->wifi_scanned_devices = current->next;
				else
					prev->next = current->next;
				s->wifi_num_devices--;
				free(current);
				current = NULL;
				return ;
			}
		}
	}
}

void	clear_lst(t_state *s, t_structype type)
{
	t_wifi_dev_info	*current;
	t_wifi_dev_info	*next;

	if (type == LE_INFO || type == ALL_INFO)
	{
		pthread_mutex_lock(&s->le_data_mutex);
		for (t_le_scan_dev_info *current = s->le_scanned_devices,
			*next; current != NULL; current = next)
		{
			next = current->next;
			free(current);
			current = NULL;
		}
		pthread_mutex_unlock(&s->le_data_mutex);
		s->le_scanned_devices = NULL;
		s->le_num_scanned_devices = 0;
	}
	if (type == HCI_INFO || type == ALL_INFO)
	{
		pthread_mutex_lock(&s->hci_data_mutex);
		for (t_hci_dev_data *current = s->hci_devices,
			*next; current != NULL; current = next)
		{
			next = current->next;
			if (current->sock_fd >= 0)
				hci_close_dev(current->sock_fd);
			free(current);
			current = NULL;
		}
		pthread_mutex_unlock(&s->hci_data_mutex);
		s->hci_devices = NULL;
		s->num_hci_devices = 0;
	}
	else if (type == WIFI_INFO || type == ALL_INFO)
	{
		pthread_mutex_lock(&s->wifi_data_mutex);
		for (current = s->wifi_scanned_devices; current != NULL; current = next)
		{
			next = current->next;
			free(current);
			current = NULL;
		}
		pthread_mutex_unlock(&s->wifi_data_mutex);
		s->wifi_scanned_devices = NULL;
		s->wifi_num_devices = 0;
	}
}
