#include "mainheader.h"

#define MS_5MIN 300000

/**

	* @brief Monitors the devices arrays and removes devices that haven't been seen for 5 minutes
 */
void	*dev_lst_monitor(void *arg)
{
	t_ScannerContext	*ctx;
	BleDeviceInfo		*bt_current;
	BleDeviceInfo		*bt_next;
	WifiDeviceInfo		*wifi_current;
	WifiDeviceInfo		*wifi_next;
	long long			current_ms;

	ctx = (t_ScannerContext *)arg;
	bt_current = NULL;
	bt_next = NULL;
	wifi_current = NULL;
	wifi_next = NULL;
	while (!pthreads_check_terminate_flag(ctx))
	{
		sleep(5);
		pthread_mutex_lock(&ctx->ble_data_mutex);
		bt_current = ctx->bt_devices;
		current_ms = timeval_to_ms();
		while (bt_current != NULL)
		{
			if (current_ms - bt_current->last_seen_time_ms > MS_5MIN)
			{
				printf("Removing device: MAC Address: %s, Last Seen: %lld\n",
					bt_current->mac_addr, bt_current->last_seen_time_ms);
				bt_next = bt_current->next;
				remove_device_lst(ctx, bt_current, BLE_INFO);
				bt_current = bt_next;
			}
			else
				bt_current = bt_current->next;
		}
		pthread_mutex_unlock(&ctx->ble_data_mutex);
		if (ctx->wifi_scan_on == true)
		{
			pthread_mutex_lock(&ctx->wifi_data_mutex);
			wifi_current = ctx->wifi_devices;
			while (wifi_current != NULL)
			{
				if (wifi_current->seen_ms_ago > MS_5MIN)
				{
					printf("Removing Wi-Fi device: MAC Address: %s, Last Seen:\
						%ld\n",
							wifi_current->mac_addr,
							wifi_current->seen_ms_ago);
					wifi_next = wifi_current->next;
					remove_device_lst(ctx, wifi_current, WIFI_INFO);
					wifi_current = wifi_next;
				}
				else
					wifi_current = wifi_current->next;
			}
			pthread_mutex_unlock(&ctx->wifi_data_mutex);
		}
	}
	return (NULL);
}
