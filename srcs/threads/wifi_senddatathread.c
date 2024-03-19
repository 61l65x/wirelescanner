#include "mainheader.h"

// Function to send data as a POST request
static int send_wifi_data_post(CURL *curl, const WifiDeviceInfo *device, int sender_id)
{
    CURLcode res;
    char full_url[1024];

    // Format the full URL with query parameters for Wi-Fi data
    snprintf(full_url, sizeof(full_url), 
             "%s?mac_address=%s&ssid=%s&seen_ms_ago=%ld&rssi_signal=%d&frequency=%.4f&is_connected=%d&sender_id=%d", 
             WIFI_URL, device->mac_addr, device->ssid, device->seen_ms_ago, 
             device->rssi, device->frequency, device->is_connected, sender_id);

    curl_easy_setopt(curl, CURLOPT_URL, full_url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        return (fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res)), -1);
    return 0;
}

// Callback function for server response
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    (void)ptr;
    (void)userdata;
    return size * nmemb;
}

void* wifi_senddata(void *arg)
{
    t_ScannerContext *ctx = (t_ScannerContext *)arg;
    CURL *curl = curl_easy_init();
    WifiDeviceInfo tempCopy = {0};

    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    if (!curl)
        return (perror("Error initializing curl"), NULL);
    
    while (!pthreads_check_terminate_flag(ctx))
    {
        pthread_mutex_lock(&ctx->wifi_data_mutex);
        if (ctx->wifi_data_updated)
        {
            for (WifiDeviceInfo *current = ctx->wifi_devices; current != NULL; current = current->next)
            {
                tempCopy = *current;
                if (send_wifi_data_post(curl, &tempCopy, 1) < 0)
                    fprintf(stderr, "Failed to send data for device %s\n", current->mac_addr);
            }
            ctx->wifi_data_updated = false;
        }
        pthread_mutex_unlock(&ctx->wifi_data_mutex);
        usleep(100);
    }
    if (curl)
        curl_easy_cleanup(curl);
    return NULL;
}