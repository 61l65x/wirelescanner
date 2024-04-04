
#include "mainheader.h"

/**
 * @brief Gets current time & returns it in milliseconds.
 * @param tv The timeval to convert.
 * @return The timeval in milliseconds.
 */
long long	timeval_to_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (long long)(tv.tv_sec) * 1000LL + (tv.tv_usec / 1000);
}

/**
 * @brief Checks the terminate flag across all threads
 * if set returns 1, else 0.
 */
int	pthreads_check_terminate_flag(t_ScannerContext *ctx)
{
	int	flag;

	if (ctx == NULL)
		return (0);
	pthread_mutex_lock(&ctx->thread_error_mutex);
	flag = ctx->terminate_flag;
	pthread_mutex_unlock(&ctx->thread_error_mutex);
	return (flag);
}

/**
 * @brief Sets the terminate flag across all threads
 * for exiting the threads.
 */
void	pthreads_set_terminate_flag(t_ScannerContext *ctx)
{
	if (ctx == NULL)
		return ;
	pthread_mutex_lock(&ctx->thread_error_mutex);
	ctx->terminate_flag = 1;
	pthread_mutex_unlock(&ctx->thread_error_mutex);
}