
#include "mainheader.h"

long long	timeval_to_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (long long)(tv.tv_sec) * 1000LL + (tv.tv_usec / 1000);
}
