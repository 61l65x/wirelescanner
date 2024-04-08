#include "mainheader.h"
#define _POSIX_C_SOURCE 200809L

_Atomic bool g_terminate_flag = false;

static void	main_signal_handler(int signo, siginfo_t *info, void *ptr)
{
	(void)info;
	(void)ptr;
	if (signo == SIGINT)
	{
		printf("SIGINT received\n");
		SET_TERMINATE_FLAG();
	}
}

void	init_signals(t_state *s)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = main_signal_handler;
	signal(SIGQUIT, SIG_IGN);
	if (sigaction(SIGINT, &sa, NULL) < 0)
	{
		perror("Error: sigaction() failed");
		exit(EXIT_FAILURE);
	}
}
