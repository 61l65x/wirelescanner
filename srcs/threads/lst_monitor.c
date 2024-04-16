#include "mainheader.h"
#define MINUTES_5 300000

static bool	not_terminated_sleep(int check_interval)
{
	if (IS_TERMINATED())
		return (false);
	sleep(check_interval);
	return (!IS_TERMINATED());
}

static void	check_wifi_devices(t_state *s)
{
	t_wifi_scan_dev_info	*current;
	t_wifi_scan_dev_info	*next;

	current = s->wifi_scanned_devices;
	while (!IS_TERMINATED() && current != NULL)
	{
		if (current->seen_ms_ago > MINUTES_5)
		{
			printf("Removing WIFI device: MAC Address: %s, Last Seen: %ld\n",
				current->mac_addr, current->seen_ms_ago);
			next = current->next;
			remove_from_lst(s, current, WIFI_INFO);
			current = next;
		}
		else
			current = current->next;
	}
}

static void	check_le_devices(t_state *s)
{
	t_le_scan_dev_info	*le_current;
	t_le_scan_dev_info	*le_next;
	long long			current_ms;

	le_current = s->le_scanned_devices;
	current_ms = timeval_to_ms();
	while (!IS_TERMINATED() && le_current != NULL)
	{
		if (current_ms - le_current->last_seen_time_ms > MINUTES_5)
		{
			printf("Removing LE device: MAC Address: %s, Last Seen:\
				%lld\n",
					le_current->mac_addr,
					le_current->last_seen_time_ms);
			le_next = le_current->next;
			remove_from_lst(s, le_current, LE_INFO);
			le_current = le_next;
		}
		else
			le_current = le_current->next;
	}
}

/*
static void	check_cl_devices(t_state *s)
{
	t_cl_inquiry_dev_info	*cl_current;
	t_cl_inquiry_dev_info	*cl_next;
	long long				current_ms;

	cl_current = s->cl_scanned_devices;
	current_ms = timeval_to_ms();
	while (!IS_TERMINATED() && cl_current != NULL)
	{
		if (current_ms - cl_current->last_seen_time_ms > MINUTES_5)
		// not seen for 5 minutes remove
		{
			printf("Removing device: MAC Address: %s, Last Seen: %lld\n",
				cl_current->mac_addr, cl_current->last_seen_time_ms);
			cl_next = cl_current->next;
			remove_dev_from_lst(s, cl_current, CL_INFO);
			cl_current = cl_next;
		}
		else
			cl_current = cl_current->next;
	}
}
*/

void	*lst_monitor_thread(void *arg)
{
	t_state	*s;

	s = (t_state *)arg;
	while (not_terminated_sleep(5))
	{
		pthread_mutex_lock(&s->le_data_mutex);
		check_le_devices(s);
		pthread_mutex_unlock(&s->le_data_mutex);
		usleep(100);
		pthread_mutex_lock(&s->wifi_data_mutex);
		check_wifi_devices(s);
		pthread_mutex_unlock(&s->wifi_data_mutex);
		usleep(100);
		// pthread_mutex_lock(&s->cl_data_mutex);
		// check_cl_devices(s);
		// pthread_mutex_unlock(&s->cl_data_mutex);
	}
	return (NULL);
}
