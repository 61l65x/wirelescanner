#ifndef THREADHEADER_H
#define THREADHEADER_H

#include <pthread.h>

typedef struct 	s_AllThreads
{
	pthread_t 	ble_scan_thread;
	pthread_t 	ble_send_thread;
	pthread_t 	wifi_send_thread;
	pthread_t 	wifi_scan_thread;
	pthread_t 	dev_lst_monitor_thread;
}				t_AllThreads;

// ALL THREADS
void* ble_scan_data_parser(void *arg);
void* bt_senddata(void *arg);
void* dev_lst_monitor(void* arg);
void* wifi_scan_data_parser(void* arg);
void* wifi_senddata(void *arg);

#endif