#include "mainheader.h"

int	ble_hci_request(t_ScannerContext *ctx, uint16_t ocf, int clen, void *cparam)
{
	struct hci_request	rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = ocf;
	rq.cparam = cparam;
	rq.clen = clen;
	rq.rparam = &ctx->blestatus;
	rq.rlen = 1;
	return (hci_send_req(ctx->bt_dev_fd, &rq, 1000));
}

static void	add_update_device(t_ScannerContext *ctx, const char *mac_addr,
		int8_t rssi)
{
	int	device_found;

	pthread_mutex_lock(&ctx->ble_data_mutex);
	device_found = 0;
	for (BleDeviceInfo *current = ctx->bt_devices; current != NULL; current = current->next)
	{
		if (strcmp(current->mac_addr, mac_addr) == 0)
		{
			current->rssi = rssi;
			current->last_seen_time_ms = timeval_to_ms(&ctx->timeVal);
			device_found = 1;
			break ;
		}
	}
	if (!device_found)
	{
		if (bt_add_device_lst(ctx, mac_addr, rssi) < 0)
			pthreads_set_terminate_flag(ctx);
	}
	pthread_mutex_unlock(&ctx->ble_data_mutex);
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

void	*ble_scan_data_parser(void *arg)
{
	t_ScannerContext			*ctx;
	le_set_scan_parameters_cp	scan_params_cp;
	le_set_event_mask_cp		event_mask_cp;
	int							i;
	le_set_scan_enable_cp		scan_cp;
	struct hci_filter			nf;
	uint8_t						buf[HCI_MAX_EVENT_SIZE];
	evt_le_meta_event			*meta_event;
	le_advertising_info			*info;
	int							len;
	uint8_t						reports_count;
	void						*offset;
					char mac_addr[18];

	ctx = (t_ScannerContext *)arg;
	// Set BLE scan parameters.
	memset(&scan_params_cp, 0, sizeof(scan_params_cp));
	scan_params_cp.type = 0x00;
	scan_params_cp.interval = htobs(0x0010);
	scan_params_cp.window = htobs(0x0010);
	scan_params_cp.own_bdaddr_type = 0x00; // Public Device Address (default).
	scan_params_cp.filter = 0x00;          // Accept all.
	if (ble_hci_request(ctx, OCF_LE_SET_SCAN_PARAMETERS,
			LE_SET_SCAN_PARAMETERS_CP_SIZE, &scan_params_cp) < 0)
		return (perror("Failed to set scan parameters data."), NULL);
	// Set BLE events report mask.
	memset(&event_mask_cp, 0, sizeof(le_set_event_mask_cp));
	i = 0;
	for (i = 0; i < 8; i++)
		event_mask_cp.mask[i] = 0xFF;
	if (ble_hci_request(ctx, OCF_LE_SET_EVENT_MASK, LE_SET_EVENT_MASK_CP_SIZE,
			&event_mask_cp) < 0)
		return (perror("Failed to set event mask."), NULL);
	// Enable scanning.
	memset(&scan_cp, 0, sizeof(scan_cp));
	scan_cp.enable = 0x01;     // Enable flag.
	scan_cp.filter_dup = 0x00; // Filtering disabled.
	if (ble_hci_request(ctx, OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE,
			&scan_cp) < 0)
		return (perror("Failed to enable scan."), NULL);
	// Get Results.
	hci_filter_clear(&nf);
	hci_filter_all_ptypes(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);
	if (setsockopt(ctx->bt_dev_fd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0)
		return (perror("Could not set socket options\n"), NULL);
	while (!pthreads_check_terminate_flag(ctx))
	{
		len = read(ctx->bt_dev_fd, buf, sizeof(buf));
		if (len >= HCI_EVENT_HDR_SIZE)
		{
			meta_event = (evt_le_meta_event *)(buf + HCI_EVENT_HDR_SIZE + 1);
			if (meta_event->subevent == EVT_LE_ADVERTISING_REPORT)
			{
				reports_count = meta_event->data[0];
				offset = meta_event->data + 1;
				while (reports_count--)
				{
					info = (le_advertising_info *)offset;
					// Print MAC address and RSSI
					ba2str(&(info->bdaddr), mac_addr);
					int8_t rssi = (int8_t) * (info->data + info->length);
						// Assuming RSSI is at the end
					print_device_info(mac_addr, rssi, info->data, info->length);
					add_update_device(ctx, mac_addr, rssi);
					offset = (uint8_t *)offset + sizeof(le_advertising_info)
						+ info->length + 1; // +1 for RSSI
				}
			}
		}
		else
		{
			perror("Cannot read from socket\n");
			break ;
		}
	}
	// Disable scanning.
	memset(&scan_cp, 0, sizeof(scan_cp));
	scan_cp.enable = 0x00; // Disable flag.
	if (ble_hci_request(ctx, OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE,
			&scan_cp) < 0)
		return (perror("Failed to disable scan."), NULL);
	return (NULL);
}
