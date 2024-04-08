#ifndef THREADHEADER_H
# define THREADHEADER_H

# include <pthread.h>

typedef struct s_thread_ids
{
	pthread_t	le_scan_id;
	pthread_t	le_send_thread;
	pthread_t	wifi_send_thread;
	pthread_t	wifi_scan_thread;
	pthread_t	lst_monitor_id;
}				t_thread_ids;

// ALL THREADS
void			*le_scan_thread(void *arg);
void			*le_send_thread(void *arg);
void			*lst_monitor_thread(void *arg);
void			*wifi_scan_thread(void *arg);
void			*wifi_senddata(void *arg);

#endif