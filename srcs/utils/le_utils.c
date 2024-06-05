
#include "wirelescanner.h"

#define EIR_NAME_SHORT 0x08    /* le shortened local name */
#define EIR_NAME_COMPLETE 0x09 /* le complete local name */

int	le_add_scanned_dev_to_lst(t_state *s, const bdaddr_t *bdaddr,
		const char *mac_addr, int8_t rssi)
{
	t_le_scan_dev_info	*new_device;

	new_device = calloc(1, sizeof(t_le_scan_dev_info));
	if (!new_device)
		return (perror("malloc in bt_add_device_lst"), -1);
	bacpy(&new_device->bdaddr, bdaddr);
	strcpy(new_device->mac_addr, mac_addr);
	new_device->last_seen_time_ms = timeval_to_ms();
	new_device->rssi = rssi;
	new_device->next = s->bt_info.le_scanned_devices;
	s->bt_info.le_scanned_devices = new_device;
	s->bt_info.le_num_scanned_devices++;
	return (0);
}

void	le_update_add_dev(t_state *s, const bdaddr_t *bdaddr, int8_t rssi)
{
	bool	device_found;
	char	mac_addr[19];

	ba2str(bdaddr, mac_addr);
	pthread_mutex_lock(&s->bt_info.le_data_mutex);
	device_found = false;
	for (t_le_scan_dev_info *current = s->bt_info.le_scanned_devices; current != NULL; current = current->next)
	{
		if (strcmp(current->mac_addr, mac_addr) == 0)
		{
			current->rssi = rssi;
			current->last_seen_time_ms = timeval_to_ms();
			device_found = true;
			break ;
		}
	}
	if (!device_found)
	{
		if (le_add_scanned_dev_to_lst(s, bdaddr, mac_addr, rssi) < 0)
			SET_TERMINATE_FLAG();
	}
	pthread_mutex_unlock(&s->bt_info.le_data_mutex);
}

/**
 * Parse name from extended inquiry response le data.
 */
void	eir_parse_name(uint8_t *eir, size_t eir_len, char *buf, size_t buf_len)
{
	size_t	offset;
	uint8_t	field_len;
	size_t	name_len;

	offset = 0;
	while (offset < eir_len)
	{
		field_len = eir[0];
		/* Check for the end of EIR */
		if (field_len == 0)
			break ;
		if (offset + field_len > eir_len)
			goto failed;
		switch (eir[1])
		{
		case EIR_NAME_SHORT:
		case EIR_NAME_COMPLETE:
			name_len = field_len - 1;
			if (name_len > buf_len)
				goto failed;
			memcpy(buf, &eir[2], name_len);
			return ;
		}
		offset += field_len + 1;
		eir += field_len + 1;
	}
failed:
	snprintf(buf, buf_len, "(unknown)");
}
