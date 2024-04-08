
#include "mainheader.h"

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
	new_device->next = s->le_scanned_devices;
	s->le_scanned_devices = new_device;
	s->le_num_scanned_devices++;
	return (0);
}

void	le_update_add_dev(t_state *s, const bdaddr_t *bdaddr, int8_t rssi)
{
	bool	device_found;
	char	mac_addr[19];

	ba2str(bdaddr, mac_addr);
	pthread_mutex_lock(&s->le_data_mutex);
	device_found = false;
	for (t_le_scan_dev_info *current = s->le_scanned_devices; current != NULL; current = current->next)
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
	pthread_mutex_unlock(&s->le_data_mutex);
}

void	print_device_info(const char *mac, int rssi, const uint8_t *data,
		size_t data_len)
{
	size_t	index;
	uint8_t	field_len;
	uint8_t	field_type;
	int		manufacturer_id;

	printf("Device: %s - RSSI: %d\n", mac, rssi);
	printf("Advertising Data: ");
	for (size_t i = 0; i < data_len; i++)
	{
		printf("%02x ", data[i]);
	}
	printf("\n");
	index = 0;
	while (index < data_len)
	{
		field_len = data[index];
		if (field_len == 0 || index + field_len >= data_len)
			break ; // Safety check
		field_type = data[index + 1];
		switch (field_type)
		{
		case 0x09: // Complete Local Name
		case 0x08: // Shortened Local Name
			printf("Device Name: %.*s\n", field_len - 1, &data[index + 2]);
			break ;
		case 0xFF: // Manufacturer Specific Data
			if (field_len >= 3)
			{ // Manufacturer ID plus at least one data byte
				manufacturer_id = data[index + 2] | (data[index + 3] << 8);
				printf("Manufacturer ID: %04x\n", manufacturer_id);
				printf("Manufacturer Data: ");
				for (size_t i = 4; i < field_len + 1; i++)
				{
					printf("%02x ", data[index + i]);
				}
				printf("\n");
			}
		case 0x03: // Complete List of 16-bit Service UUIDs
			printf("16-bit Service UUIDs: ");
			for (size_t i = 2; i < field_len + 1; i += 2)
			{
				printf("%02x%02x ", data[index + i + 1], data[index + i]);
			}
			printf("\n");
			break ;
		case 0x07: // Complete List of 128-bit Service UUIDs
			printf("128-bit Service UUIDs: ");
			for (size_t i = 2; i < field_len + 1; i += 16)
			{
				for (int j = 15; j >= 0; --j)
				{
					printf("%02x", data[index + i + j]);
					if (j == 12 || j == 10 || j == 8 || j == 6)
						printf("-");
				}
				printf(" ");
			}
			printf("\n");
			break ;
			// Add more cases for other field types if needed
		}
		index += field_len + 1;
	}
	printf("\n");
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
