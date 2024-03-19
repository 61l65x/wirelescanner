#include "mainheader.h"

/**
 * @brief Adds a device to the wifi device list.
 * add to begin so dont have to loop to end fast
 * @param ctx The scanner context.
 * @param bss The bss info of the device.
*/
int wifi_add_device_lst(t_ScannerContext *ctx, struct bss_info *bss)
{
    WifiDeviceInfo *new_device = calloc(1, sizeof(WifiDeviceInfo));
    if (!new_device)
	{
        perror("malloc in add_wifi_device");
        return -1;
    }
    bssid_to_string(bss->bssid, new_device->mac_addr);
    snprintf(new_device->ssid, sizeof(new_device->ssid), "%s", bss->ssid);
    new_device->seen_ms_ago = bss->seen_ms_ago;
    new_device->rssi = bss->signal_mbm / 100; // Assuming signal_mbm is in millibels and you want it in dB
    new_device->frequency = (float)bss->frequency / 1000.0;
    new_device->is_connected = bss->status == BSS_ASSOCIATED ? 1 : 0;
    new_device->next = ctx->wifi_devices;
    ctx->wifi_devices = new_device;
    return 0;
}

/**
 * @brief Adds a device to the bluetooth device list.
 * add to begin so dont have to loop to end fast
 * @param ctx The scanner context.
 * @param mac_addr The mac address of the device.
 * @param rssi The rssi of the device.
*/
int bt_add_device_lst(t_ScannerContext *ctx, const char *mac_addr, int8_t rssi)
{
    BleDeviceInfo *new_device = calloc(1, sizeof(BleDeviceInfo));
    if (!new_device)
    {
        perror("malloc in bt_add_device_lst");
        return -1;
    }
    strcpy(new_device->mac_addr, mac_addr);
    new_device->last_seen_time_ms = timeval_to_ms(&ctx->timeVal);
    new_device->rssi = rssi;
    new_device->next = ctx->bt_devices;
    ctx->bt_devices = new_device;
    ctx->bt_num_devices++;
    return 0;
}


/**
 * @brief Generalized function to remove a device from wifi or bluetooth list.
 * @param ctx The scanner context.
 * @param device_to_remove The device to remove.
 * @param type The type of the device to remove.
*/
void remove_device_lst(t_ScannerContext *ctx, void *device_to_remove, t_structype type)
{
    if (type == BLE_INFO)
    {
        BleDeviceInfo *current = ctx->bt_devices;
        BleDeviceInfo *previous = NULL;

        while (current != NULL)
        {
            if (current == (BleDeviceInfo *)device_to_remove)
            {
                if (previous == NULL)
                    ctx->bt_devices = current->next;
                else 
                    previous->next = current->next;
                free(current);
                ctx->bt_num_devices--;
                return;
            }
            previous = current;
            current = current->next;
        }
    }
    else if (type == WIFI_INFO)
    {
        WifiDeviceInfo *current = ctx->wifi_devices;
        WifiDeviceInfo *previous = NULL;

        while (current != NULL)
        {
            if (current == (WifiDeviceInfo *)device_to_remove)
            {
                if (previous == NULL)
                    ctx->wifi_devices = current->next;
                else 
                    previous->next = current->next;
                free(current);
                ctx->wifi_num_devices--;
                return;
            }
            previous = current;
            current = current->next;
        }
    }
}


/**
 * @brief Frees the list of dynamic allocated devices
 * either bluetooth or wifi struct.
 * @param ctx The scanner context.
 * @param free_wifi 0 for bluetooth, 1 for wifi.
*/
void free_devices_lst(t_ScannerContext *ctx,  t_structype type)
{
    if (type == BLE_INFO)
    {
        BleDeviceInfo *current = ctx->bt_devices;
        BleDeviceInfo *next;

        while (current != NULL)
        {
            next = current->next;
            free(current);
            current = next;
        }
        ctx->bt_devices = NULL;
        ctx->bt_num_devices = 0;
    }
    else if (type == WIFI_INFO)
    {
        WifiDeviceInfo *current = ctx->wifi_devices;
        WifiDeviceInfo *next;

        while (current != NULL)
        {
            next = current->next;
            free(current);
            current = next;
        }
        ctx->wifi_devices = NULL;
        ctx->bt_num_devices = 0;
    }
}
