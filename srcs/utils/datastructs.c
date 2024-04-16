#include "mainheader.h"

typedef struct s_generic_struct
{
	void	*next;
}			t_generic_struct;

void	remove_dev_from_lst(t_state *s, void *dev_to_remove, t_structype type)
{
	void	**head;
	int		*count;
	void	**current;
	void	**prev;

	current = NULL;
	prev = NULL;
	head = NULL;
	count = NULL;
	current = NULL;
	switch (type)
	{
	case LE_INFO:
		head = (void **)&s->le_scanned_devices;
		count = &s->le_num_scanned_devices;
		break ;
	case CL_INFO:
		head = (void **)&s->cl_scanned_devices;
		count = &s->cl_num_scanned_devices;
		break ;
	case HCI_INFO:
		head = (void **)&s->hci_ifaces;
		count = &s->num_hci_devices;
		break ;
	case WIFI_INFO:
		head = (void **)&s->wifi_scanned_devices;
		count = &s->wifi_num_scanned_devices;
		break ;
	case NTWRK_INFO:
		break ;
	default:
		return ;
	}
	for (current = head; *current; prev = current,
		current = &((t_generic_struct *)(*current))->next)
	{
		if (*current == dev_to_remove)
		{
			if (prev == NULL)
				*head = ((t_generic_struct *)(*current))->next;
			else
				*((void **)(*prev)) = ((t_generic_struct *)(*current))->next;
			(*count)--;
			if (type == HCI_INFO
				&& ((t_bt_hci_iface *)(*current))->sock_fd >= 0)
				hci_close_dev(((t_bt_hci_iface *)(*current))->sock_fd);
			free(*current);
			break ;
		}
	}
}

void	clear_lst(t_state *s, t_structype type)
{
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
		for (t_bt_hci_iface *current = s->hci_ifaces,
			*next; current != NULL; current = next)
		{
			next = current->next;
			if (current->sock_fd >= 0)
				hci_close_dev(current->sock_fd);
			free(current);
			current = NULL;
		}
		pthread_mutex_unlock(&s->hci_data_mutex);
		s->hci_ifaces = NULL;
		s->num_hci_devices = 0;
	}
	if (type == CL_INFO || type == ALL_INFO)
	{
		pthread_mutex_lock(&s->cl_data_mutex);
		for (t_cl_inquiry_dev_info *current = s->cl_scanned_devices,
			*next; current != NULL; current = next)
		{
			next = current->next;
			free(current);
		}
		pthread_mutex_unlock(&s->cl_data_mutex);
		s->cl_scanned_devices = NULL;
		s->cl_num_scanned_devices = 0;
	}
	if (type == WIFI_INFO || type == ALL_INFO)
	{
		pthread_mutex_lock(&s->wifi_data_mutex);
		for (t_wifi_scan_dev_info *current = s->wifi_scanned_devices,
			*next; current != NULL; current = next)
		{
			next = current->next;
			free(current);
			current = NULL;
		}
		pthread_mutex_unlock(&s->wifi_data_mutex);
		s->wifi_scanned_devices = NULL;
		s->wifi_num_scanned_devices = 0;
	}
}
