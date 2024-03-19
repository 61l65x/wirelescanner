#ifndef MAINHEADER_H
#define MAINHEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/time.h>
#include "wifi_scan_api.h"
#include "threadheader.h"
#include <stdbool.h>
#include "messages.h"

#define BT_URL "http://192.168.8.109:8000/bluetooth"
#define WIFI_URL "http://192.168.8.109:8000/wifi"


typedef enum e_datastructs
{
    BLE_INFO,
    WIFI_INFO
} t_structype;


typedef struct BleDeviceInfo
{
    char mac_addr[19];
    char name[100];
    long long last_seen_time_ms;
    int8_t rssi;
    struct BleDeviceInfo *next;
} BleDeviceInfo;

typedef struct WifiDeviceInfo
{
    char    mac_addr[19];
    char    ssid[33];
    long    seen_ms_ago;
    int8_t  rssi;
    float     frequency;
    bool    is_connected;
    struct WifiDeviceInfo *next;
} WifiDeviceInfo;

typedef struct t_ScannerContext
{
    BleDeviceInfo *bt_devices;
    WifiDeviceInfo *wifi_devices;
    char wifi_iface[16];
    int  wifi_num_devices;
    bool wifi_data_updated;
    bool wifi_scan_on;
    int bt_num_devices;
    int bt_dev_fd;
    int bt_dev_id;
    int blestatus;
    pthread_mutex_t thread_error_mutex;
    pthread_mutex_t ble_data_mutex;
    pthread_mutex_t wifi_data_mutex;
    volatile int terminate_flag;
    struct timeval timeVal;
} t_ScannerContext;

// data structures
int     wifi_add_device_lst(t_ScannerContext *ctx, struct bss_info *bss);
int     bt_add_device_lst(t_ScannerContext *ctx, const char *mac_addr, int8_t rssi);
void    remove_device_lst(t_ScannerContext *ctx, void *device_to_remove, t_structype type);
void    free_devices_lst(t_ScannerContext *ctx,  t_structype type);

//thread utils
void           pthreads_set_terminate_flag(t_ScannerContext *ctx);
int          pthreads_check_terminate_flag(t_ScannerContext *ctx);
long long       timeval_to_ms(struct timeval *tv);
//wifi utils
int         get_active_network_interface(char *buffer, size_t buffer_size);
const char *bssid_to_string(const uint8_t bssid[BSSID_LENGTH], char bssid_string[BSSID_STRING_LENGTH]);


#endif