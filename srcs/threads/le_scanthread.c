#include "mainheader.h"

static int	set_le_socket_opts(t_hci_dev_data *hci_dev)
{
	struct hci_filter	nf;
	int					flags;

	hci_filter_clear(&nf);
	hci_filter_all_ptypes(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);
	if (setsockopt(hci_dev->sock_fd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0)
		return (perror("Could not set socket options\n"), FAIL);
	// Setting non-blocking mode to le read socket
	flags = fcntl(hci_dev->sock_fd, F_GETFL, 0);
	if (flags < 0)
		return (perror("fcntl get failed"), FAIL);
	if (fcntl(hci_dev->sock_fd, F_SETFL, flags | O_NONBLOCK) < 0)
		return (perror("fcntl set failed"), FAIL);
	return (OK);
}

static void	scan_loop(t_state *s, t_hci_dev_data *hci_dev)
{
	int8_t				rssi;
	le_advertising_info	*info;
	int					bytes_read;
	uint8_t				buf[HCI_MAX_EVENT_SIZE];
	evt_le_meta_event	*meta_event;
	uint8_t				reports_count;
	void				*offset;
	char				macaddr[18];
	char				name[248];

	while (!IS_TERMINATED())
	{
		bytes_read = read(hci_dev->sock_fd, buf, sizeof(buf));
		if (bytes_read < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				usleep(100);
				continue ;
			}
			else
			{
				perror("Cannot read from socket");
				break ;
			}
		}
		if (bytes_read >= HCI_EVENT_HDR_SIZE)
		{
			meta_event = (evt_le_meta_event *)(buf + HCI_EVENT_HDR_SIZE + 1);
			if (meta_event->subevent == EVT_LE_ADVERTISING_REPORT)
			{
				reports_count = meta_event->data[0];
				offset = meta_event->data + 1;
				while (reports_count--)
				{
					info = (le_advertising_info *)offset;
					rssi = (int8_t) * (info->data + info->length);
					ba2str(&info->bdaddr, macaddr);
					// Assuming RSSI is at the end
					eir_parse_name(info->data, info->length, name,
						sizeof(name));
					printf("Device: %s, Name: %s, RSSI: %d dBm\n", macaddr,
						name, rssi);
					// print_device_info(macaddr, rssi, info->data,
					//	info->length);
					le_update_add_dev(s, &info->bdaddr, rssi);
					offset = (uint8_t *)offset + sizeof(le_advertising_info)
						+ info->length + 1; // +1 for RSSI
				}
			}
		}
	}
	printf("Leaving scan loop le \n");
}

void	*le_scan_thread(void *arg)
{
	t_state			*s;
	t_hci_dev_data	*hci_dev;

	s = (t_state *)arg;
	printf("before job assing le\n");
	hci_dev = get_hci_dev_for_job(s, JOB_SCAN_LE_DATA);
	if (!hci_dev)
		return (perror("get_hci_dev_for_job le_scan_thread"), NULL);
	printf("Starting LE scan controller %d\n", hci_dev->sock_fd);
	// set_le_scan_params_masks(hci_dev);
	hci_le_set_scan_parameters(hci_dev->sock_fd, 0x00, htobs(0x0010),
		htobs(0x0010), 0x00, 0x00, 1000);
	hci_le_set_scan_enable(hci_dev->sock_fd, 0x01, 0x00, 100);
	set_le_socket_opts(hci_dev);
	scan_loop(s, hci_dev);
	hci_le_set_scan_enable(hci_dev->sock_fd, 0x00, 0x00, 100);
	return (NULL);
}
