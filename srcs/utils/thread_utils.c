
#include "mainheader.h"

long long	timeval_to_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (long long)(tv.tv_sec) * 1000LL + (tv.tv_usec / 1000);
}

int	pthreads_check_terminate_flag(t_state *ctx)
{
	int	flag;

	if (ctx == NULL)
		return (0);
	pthread_mutex_lock(&ctx->thread_error_mutex);
	flag = ctx->terminate_flag;
	pthread_mutex_unlock(&ctx->thread_error_mutex);
	return (flag);
}

void	pthreads_set_terminate_flag(t_state *ctx)
{
	if (ctx == NULL)
		return ;
	pthread_mutex_lock(&ctx->thread_error_mutex);
	ctx->terminate_flag = 1;
	pthread_mutex_unlock(&ctx->thread_error_mutex);
}