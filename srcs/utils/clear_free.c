#include "wirelescanner.h"

typedef struct s_generic_struct
{
	void	*next;
}			t_generic_struct;

void remove_from_lst(t_state *s, void *to_remove, t_structype type)
{
    void **head = NULL;
    int *count = NULL;

    switch (type)
	{
        case LE_INFO: head = (void **)&s->bt_info.le_scanned_devices; count = &s->bt_info.le_num_scanned_devices; break;
        case CL_INFO: head = (void **)&s->bt_info.cl_scanned_devices; count = &s->bt_info.cl_num_scanned_devices; break;
        case HCI_INFO: head = (void **)&s->bt_info.hci_ifaces; count = &s->bt_info.num_hci_devices; break;
        case WIFI_INFO: head = (void **)&s->ntwrk_info.wifi_scanned_devices; count = &s->ntwrk_info.wifi_num_scanned_devices; break;
        case NTWRK_INFO: return;
        default: return;
    }

    for (void **current = head, **prev = NULL; *current; prev = current, current = &((t_generic_struct *)(*current))->next)
	{
        if (*current == to_remove)
		{
            *head = prev ? *((void **)(*prev)) = ((t_generic_struct *)(*current))->next : ((t_generic_struct *)(*current))->next;
            if (type == HCI_INFO && ((t_bt_hci_iface *)(*current))->sock_fd >= 0)
                hci_close_dev(((t_bt_hci_iface *)(*current))->sock_fd);
            (*count)--;
            free(*current);
			*current = NULL;
            break;
        }
    }
}


void	clear_lst(t_state *s, t_structype type)
{
	if (type == LE_INFO || type == ALL_INFO)
	{
		pthread_mutex_lock(&s->bt_info.le_data_mutex);
		for (t_le_scan_dev_info *current = s->bt_info.le_scanned_devices,
			*next; current != NULL; current = next)
		{
			next = current->next;
			free(current);
			current = NULL;
		}
		pthread_mutex_unlock(&s->bt_info.le_data_mutex);
		s->bt_info.le_scanned_devices = NULL;
		s->bt_info.le_num_scanned_devices = 0;
	}
	if (type == HCI_INFO || type == ALL_INFO)
	{
		pthread_mutex_lock(&s->bt_info.hci_data_mutex);
		for (t_bt_hci_iface *current = s->bt_info.hci_ifaces,
			*next; current != NULL; current = next)
		{
			next = current->next;
			if (current->sock_fd >= 0)
				hci_close_dev(current->sock_fd);
			free(current);
			current = NULL;
		}
		pthread_mutex_unlock(&s->bt_info.hci_data_mutex);
		s->bt_info.hci_ifaces = NULL;
		s->bt_info.num_hci_devices = 0;
	}
	if (type == CL_INFO || type == ALL_INFO)
	{
		pthread_mutex_lock(&s->bt_info.cl_data_mutex);
		for (t_cl_inquiry_dev_info *current = s->bt_info.cl_scanned_devices,
			*next; current != NULL; current = next)
		{
			next = current->next;
			free(current);
		}
		pthread_mutex_unlock(&s->bt_info.cl_data_mutex);
		s->bt_info.cl_scanned_devices = NULL;
		s->bt_info.cl_num_scanned_devices = 0;
	}
	if (type == WIFI_INFO || type == ALL_INFO)
	{
		pthread_mutex_lock(&s->wifi_data_mutex);
		for (t_wifi_scan_dev_info *current = s->ntwrk_info.wifi_scanned_devices,
			*next; current != NULL; current = next)
		{
			next = current->next;
			free(current);
			current = NULL;
		}
		pthread_mutex_unlock(&s->wifi_data_mutex);
		s->ntwrk_info.wifi_scanned_devices = NULL;
		s->ntwrk_info.wifi_num_scanned_devices = 0;
	}
}
