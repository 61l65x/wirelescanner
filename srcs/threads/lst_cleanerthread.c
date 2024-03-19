#include "mainheader.h"

/**
 * @brief Monitors the devices array and removes devices that haven't been seen for 5 minutes
*/
void* dev_lst_monitor(void* arg)
{
    t_ScannerContext*   ctx = (t_ScannerContext*)arg;
    BleDeviceInfo*      bt_current = NULL;
    BleDeviceInfo*      bt_next = NULL;
    WifiDeviceInfo*     wifi_current = NULL;
    WifiDeviceInfo*     wifi_next = NULL;


    while (!pthreads_check_terminate_flag(ctx))
    {
        sleep(5);// intervall for checking devices in the lists
        pthread_mutex_lock(&ctx->ble_data_mutex);
        bt_current = ctx->bt_devices;
        gettimeofday(&ctx->timeVal, NULL);
        long long current_ms = timeval_to_ms(&ctx->timeVal);
        while (bt_current != NULL)
        {
            if (current_ms -  bt_current->last_seen_time_ms > 300000) // not seen for 5 minutes remove
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

        //Monitor Wi-Fi devices
        if (ctx->wifi_scan_on == true)
        {
            pthread_mutex_lock(&ctx->wifi_data_mutex);
            wifi_current = ctx->wifi_devices;
            while (wifi_current != NULL)
            {
                if (wifi_current->seen_ms_ago > 300000) // not seen for 5 minutes remove
                {
                    printf("Removing Wi-Fi device: MAC Address: %s, Last Seen: %ld\n", 
                    wifi_current->mac_addr, wifi_current->seen_ms_ago);
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
    return NULL;
}
