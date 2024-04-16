#include "mainheader.h"
#include "threadheader.h"

int	cl_add_scanned_dev_to_lst(t_state *s, const bdaddr_t *bdaddr,
		const char *mac_addr, uint8_t *cod, int8_t rssi)
{
	t_cl_inquiry_dev_info	*new_device;

	new_device = calloc(1, sizeof(t_cl_inquiry_dev_info));
	if (!new_device)
		return (perror("calloc in cl_add_scanned_dev_to_lst"), -1);
	bacpy(&new_device->bdaddr, bdaddr);
	strcpy(new_device->mac_addr, mac_addr);
	memcpy(new_device->cod, cod, 3);
	new_device->last_seen_time_ms = timeval_to_ms();
	new_device->rssi = rssi;
	new_device->next = s->cl_scanned_devices;
	s->cl_scanned_devices = new_device;
	s->cl_num_scanned_devices++;
	return (0);
}

void	cl_update_add_dev(t_state *s, const bdaddr_t *bdaddr, uint8_t *cod,
		int8_t rssi)
{
	bool	device_found;
	char	mac_addr[19];

	ba2str(bdaddr, mac_addr);
	pthread_mutex_lock(&s->cl_data_mutex);
	device_found = false;
	for (t_cl_inquiry_dev_info *current = s->cl_scanned_devices; current != NULL; current = current->next)
	{
		if (strcmp(current->mac_addr, mac_addr) == 0)
		{
			memcpy(current->cod, cod, 3); // Update the Class of Device
			current->rssi = rssi;
			current->last_seen_time_ms = timeval_to_ms();
			device_found = true;
			break ;
		}
	}
	if (!device_found)
	{
		if (cl_add_scanned_dev_to_lst(s, bdaddr, mac_addr, cod, rssi) < 0)
			SET_TERMINATE_FLAG();
	}
	pthread_mutex_unlock(&s->cl_data_mutex);
}

/* The reaction for the SIGINT can be little slow
 * because the inquiry is blocking the main thread.
 */
void	*cl_scan_thread(void *arg)
{
	inquiry_info	ii[255] = {0};
	char			name[248] = {0};
	uint8_t			cod[3] = {0};
	int8_t			rssi;
	int16_t			handle;
	int				len;
	int				max_rsp;
	int				num_rsp;
	int				flags;
	t_bt_hci_iface	*hci;
	t_state			*s;
	inquiry_info	*ii_ptrs[255];

	for (int i = 0; i < 255; ++i)
		ii_ptrs[i] = &ii[i];
	s = (t_state *)arg;
	hci = get_hci_for_job(s, HCI_JOB_SCAN_CLASSIC_DATA);
	if (!hci)
		return (perror("get_hci_dev_for_job cl_scan_thread"), NULL);
	printf("Starting classic scan controller %d\n", hci->sock_fd);
	while (!IS_TERMINATED())
	{
		handle = 0;
		len = 8;                  // Inquiry length
		max_rsp = 255;            // Maximum number of devices to discover
		flags = IREQ_CACHE_FLUSH; // Flush cache during inquiry
		if ((num_rsp = hci_inquiry(hci->dev_id, len, max_rsp, NULL, ii_ptrs,
					flags)) < 0)
			return (perror("hci_inquiry classic"), NULL);
		printf("After inquiry num rsp %d \n ", num_rsp);
		for (int i = 0; i < num_rsp; i++)
		{
			memset(name, 0, sizeof(name));
			if (hci_read_remote_name(hci->sock_fd, &ii[i].bdaddr, 248, name,
					0) < 0)
				strcpy(name, "[unknown]");
			printf("Found classic device: %s %s\n", ii[i].bdaddr.b, name);
			if (hci_read_class_of_dev(hci->sock_fd, cod, 0) < 0)
			{
				fprintf(stderr, "Could not read CoD for device %s\n",
					ii[i].bdaddr.b);
				continue ;
			}
			if (hci_read_rssi(hci->sock_fd, handle, &rssi, 1000) < 0)
				fprintf(stderr, "Could not read RSSI\n");
			else
				printf("RSSI: %d dBm\n", (int)rssi);
			printf("%s  %s  CoD: 0x%02x%02x%02x  RSSI: %d dBm\n",
				ii[i].bdaddr.b, name, cod[2], cod[1], cod[0], (int)rssi);
			cl_update_add_dev(s, &ii[i].bdaddr, cod, rssi);
		}
		sleep(1);
	}
	printf("Leaving scan loop classic\n");
	return (NULL);
}
