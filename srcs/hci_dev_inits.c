
#include "mainheader.h"
#include "threadheader.h"

static bool	is_le_capable(struct hci_dev_info *di)
{
	return (di->type & (1 << HCI_PRIMARY) && (di->features[0] & LMP_LE
			|| di->features[7] & LMP_HOST_LE));
}

// valgrind shows leak when dev_id passed but the certain device cant be turned on if not passed
static void	rfkill_unblock_bt(int sock_fd, int dev_id, bool *rfkill_attempted)
{
	if (ioctl(sock_fd, HCIDEVUP, dev_id) < 0)
	{
		if (errno == ERFKILL)
		{
			fprintf(stderr, RFKILL_ERR_MSG);
			if (system("rfkill unblock bluetooth") != 0)
				fprintf(stderr, RFKILL_UNBLOCK_ERR_MSG);
			else
			{
				fprintf(stderr, RFKILL_UNBLOCK_SUCCESS_MSG);
				*rfkill_attempted = true;
				sleep(5);
			}
		}
		else
			perror(BT_UP_ALRDY);
	}
}

static void	check_hci_capabilities(t_state *s, struct hci_dev_info *di,
		t_hci_dev_data *new_dev)
{
	new_dev->job_mask = 0;
	if (di->type & (1 << HCI_PRIMARY))
		new_dev->job_mask |= JOB_MASK_SCAN_CLASSIC_DATA;
	if (is_le_capable(di))
		new_dev->job_mask |= JOB_MASK_SCAN_LE_DATA;
	new_dev->job_mask |= JOB_MASK_SEND_DATA;
}

int	add_hci_dev_to_lst(t_state *s, struct hci_dev_info *di, int fd)
{
	t_hci_dev_data	*new_dev;

	new_dev = calloc(1, sizeof(t_hci_dev_data));
	if (!new_dev)
		return (perror(ALLOC_ERR_MSG), -1);
	new_dev->dev_id = di->dev_id;
	bacpy(&new_dev->bdaddr, &di->bdaddr);
	ba2str(&di->bdaddr, new_dev->mac_addr);
	if (hci_read_local_name(fd, 248, new_dev->local_name, 0) < 0)
		strcpy(new_dev->local_name, "[unknown]");
	else
		new_dev->local_name[sizeof(new_dev->local_name) - 1] = '\0';
	check_hci_capabilities(s, di, new_dev);
	new_dev->dev_id = di->dev_id;
	new_dev->sock_fd = fd;
	new_dev->next = s->hci_devices;
	s->hci_devices = new_dev;
	s->num_hci_devices++;
	return (0);
}
void	print_hci_flags(unsigned int flags)
{
	if (flags & (1 << HCI_UP))
		printf("HCI_UP is set\n");
	if (flags & (1 << HCI_INIT))
		printf("HCI_INIT is set\n");
	if (flags & (1 << HCI_RUNNING))
		printf("HCI_RUNNING is set\n");
	if (flags & (1 << HCI_PSCAN))
		printf("HCI_PSCAN is set\n");
	if (flags & (1 << HCI_ISCAN))
		printf("HCI_ISCAN is set\n");
	if (flags & (1 << HCI_AUTH))
		printf("HCI_AUTH is set\n");
	if (flags & (1 << HCI_ENCRYPT))
		printf("HCI_ENCRYPT is set\n");
	if (flags & (1 << HCI_INQUIRY))
		printf("HCI_INQUIRY is set\n");
	if (flags & (1 << HCI_RAW))
		printf("HCI_RAW is set\n");
}

int	init_hci_devices(t_state *s)
{
	struct hci_dev_info	di;
	bool				rfkill_attempted;
	bool				any_device_found;

	any_device_found = false;
	rfkill_attempted = false;
	for (int dev_id = 0, sock_fd = 0; dev_id < MAX_HCI_DEVICES; dev_id++)
	{
		memset(&di, 0, sizeof(di));
		sock_fd = hci_open_dev(dev_id);
		if (sock_fd < 0)
			continue ;
		di.dev_id = dev_id;
		any_device_found = true;
		if (ioctl(sock_fd, HCIGETDEVINFO, &di) < OK)
			return (perror(HCI_DEVINFO_ERR_MSG), close(sock_fd), clear_lst(s,
					HCI_INFO), -1);
		if (!(di.flags & HCI_UP) && !rfkill_attempted)
			rfkill_unblock_bt(sock_fd, dev_id, &rfkill_attempted);
		print_hci_flags(di.flags);
		if (add_hci_dev_to_lst(s, &di, sock_fd) != OK)
			return (clear_lst(s, HCI_INFO), FAIL);
	}
	if (!any_device_found)
		return (perror(BT_HCI_ERR_MSG), FAIL);
	return (0);
}
