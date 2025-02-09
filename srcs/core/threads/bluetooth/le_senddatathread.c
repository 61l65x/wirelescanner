#include "wirelescanner.h"

// Function to send data as a POST request
static int	send_data_post(CURL *curl, const char *mac_address, int rssi_signal,
		long long seen_ms_ago, int sender_id)
{
	CURLcode	res;
	char		full_url[1024];

	snprintf(full_url, sizeof(full_url),
		"%s?mac_address=%s&rssi_signal=%d&seen_ms_ago=%lld&sender_id=%d",
		BT_URL, mac_address, rssi_signal, seen_ms_ago, sender_id);
	curl_easy_setopt(curl, CURLOPT_URL, full_url);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
		return (fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res)), -1);
	return (0);
}

// Callback function for server response
static size_t	write_callback(char *ptr, size_t size, size_t nmemb,
		void *userdata)
{
	(void)ptr;
	(void)userdata;
	return (size * nmemb);
}

void	*le_send_thread(void *arg)
{
	t_state	*ctx;
	CURL	*curl;

	ctx = (t_state *)arg;
	curl = curl_easy_init();
	// curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	if (!curl)
		return (perror("Error initializing curl"), NULL);
	while (!IS_TERMINATED())
	{
		pthread_mutex_lock(&ctx->bt_info.le_data_mutex);
		for (t_le_scan_dev_info *current = ctx->bt_info.le_scanned_devices; current != NULL; current = current->next)
		{
			pthread_mutex_unlock(&ctx->bt_info.le_data_mutex);
			if (send_data_post(curl, current->mac_addr, current->rssi,
					timeval_to_ms() - current->last_seen_time_ms, 1) < 0)
				fprintf(stderr, "Failed to send data for device %s\n",
					current->mac_addr);
			pthread_mutex_lock(&ctx->bt_info.le_data_mutex);
		}
		pthread_mutex_unlock(&ctx->bt_info.le_data_mutex);
		usleep(100);
	}
	if (curl)
		curl_easy_cleanup(curl);
	return (NULL);
}
