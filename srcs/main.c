#include "mainheader.h"

static int	init_pthreads(t_thread_ids *t, t_state *ctx)
{
	if (pthread_mutex_init(&ctx->le_data_mutex, NULL) != 0
		|| pthread_mutex_init(&ctx->hci_data_mutex, NULL) != 0)
		return (1);
	if (pthread_create(&t->le_scan_id, NULL, le_scan_thread, ctx) != 0
		|| pthread_create(&t->lst_monitor_id, NULL, lst_monitor_thread,
			ctx) != 0)
		//|| pthread_create(&t->le_send_thread, NULL, le_send_thread,ctx) != 0)
		return (1);
	if (ctx->wifi_scan_on)
	{
		if (pthread_mutex_init(&ctx->wifi_data_mutex, NULL) != 0
			|| pthread_create(&t->wifi_scan_thread, NULL, wifi_scan_thread,
				ctx) != 0 || pthread_create(&t->wifi_send_thread, NULL,
				wifi_senddata, ctx) != 0)
			return (1);
	}
	return (0);
}

static int	handle_arguments(int ac, char **av, t_state *ctx)
{
	if (ac == 2 && !strcmp(av[1], "wifi"))
		ctx->wifi_scan_on = true;
	else
		printf(ARG_OPTIONS);
	return (0);
}

static void	cleanup(t_state *ctx, t_thread_ids *threads)
{
	pthread_join(threads->le_scan_id, NULL);
	pthread_join(threads->lst_monitor_id, NULL);
	pthread_join(threads->le_send_thread, NULL);
	clear_lst(ctx, ALL_INFO);
	pthread_mutex_destroy(&ctx->le_data_mutex);
	pthread_mutex_destroy(&ctx->hci_data_mutex);
	if (ctx->wifi_scan_on)
	{
		pthread_join(threads->wifi_send_thread, NULL);
		pthread_join(threads->wifi_scan_thread, NULL);
		pthread_mutex_destroy(&ctx->wifi_data_mutex);
		clear_lst(ctx, WIFI_INFO);
	}
}

int	main(int ac, char **av)
{
	t_state			state;
	t_thread_ids	threads;

	state = (t_state){0};
	threads = (t_thread_ids){0};
	init_signals(&state);
	handle_arguments(ac, av, &state);
	if (init_hci_devices(&state) != 0)
	{
		return (perror(BT_HCI_ERR_MSG), EXIT_FAILURE);
	}
	if (state.wifi_scan_on)
	{
		if (get_active_network_interface(state.wifi_iface,
				sizeof(state.wifi_iface)) != 0)
			perror(NTWRK_IFACE_ERR_MSG);
	}
	if (init_pthreads(&threads, &state) != 0)
		perror(PTHREAD_ERR_MSG);
	cleanup(&state, &threads);
	return (0);
}
