#include "mainheader.h"

static int	init_bt_hci(t_ScannerContext *ctx)
{
	if ((ctx->bt_dev_id = hci_get_route(NULL)) < 0)
		return (-1);
	if ((ctx->bt_dev_fd = hci_open_dev(ctx->bt_dev_id)) < 0)
		return (-1);
	if (ioctl(ctx->bt_dev_fd, HCIDEVUP, NULL) < 0)
	{
		if (errno != EALREADY)
			return (hci_close_dev(ctx->bt_dev_fd), -1);
	}
	return (0);
}

/**
 * @brief Initializes the pthreads and mutexes.
 * wifi only if the wifi_scan_on flag is set.
 * @param t The thread structure.
 * @param ctx The scanner context.
 */
static int	init_pthreads(t_AllThreads *t, t_ScannerContext *ctx)
{
	if (pthread_mutex_init(&ctx->ble_data_mutex, NULL) != 0
		|| pthread_mutex_init(&ctx->thread_error_mutex, NULL) != 0)
		return (1);
	if (pthread_create(&t->ble_scan_thread, NULL, ble_scan_data_parser,
			ctx) != 0 || pthread_create(&t->dev_lst_monitor_thread, NULL,
			dev_lst_monitor, ctx) != 0 || pthread_create(&t->ble_send_thread,
			NULL, bt_senddata, ctx) != 0)
		return (1);
	if (ctx->wifi_scan_on)
	{
		if (pthread_mutex_init(&ctx->wifi_data_mutex, NULL) != 0
			|| pthread_create(&t->wifi_scan_thread, NULL, wifi_scan_data_parser,
				ctx) != 0 || pthread_create(&t->wifi_send_thread, NULL,
				wifi_senddata, ctx) != 0)
			return (1);
	}
	return (0);
}

static int	handle_arguments(int ac, char **av, t_ScannerContext *ctx)
{
	if (ac == 2 && !strcmp(av[1], "wifi"))
		ctx->wifi_scan_on = true;
	else
		printf(ARG_OPTIONS);
	return (0);
}

// Cleans up the program
static void	cleanup(t_ScannerContext *ctx, t_AllThreads *threads)
{
	// Wait for all threads to finish || to terminate.
	pthread_join(threads->ble_scan_thread, NULL);
	pthread_join(threads->dev_lst_monitor_thread, NULL);
	pthread_join(threads->ble_send_thread, NULL);
	pthread_mutex_destroy(&ctx->thread_error_mutex);
	pthread_mutex_destroy(&ctx->ble_data_mutex);
	if (ctx->wifi_scan_on)
	{
		pthread_join(threads->wifi_send_thread, NULL);
		pthread_join(threads->wifi_scan_thread, NULL);
		pthread_mutex_destroy(&ctx->wifi_data_mutex);
		free_devices_lst(ctx, WIFI_INFO);
	}
	free_devices_lst(ctx, BLE_INFO);
	hci_close_dev(ctx->bt_dev_fd);
}

int	main(int ac, char **av)
{
	t_ScannerContext	ctx;
	t_AllThreads		t;

	ctx = (t_ScannerContext){0};
	t = (t_AllThreads){0};
	handle_arguments(ac, av, &ctx);
	if (init_bt_hci(&ctx) != 0)
	{
		return (perror(BT_HCI_ERR_MSG), EXIT_FAILURE);
	}
	if (ctx.wifi_scan_on)
	{
		if (get_active_network_interface(ctx.wifi_iface,
				sizeof(ctx.wifi_iface)) != 0)
			perror(NTWRK_IFACE_ERR_MSG);
	}
	if (init_pthreads(&t, &ctx) != 0)
		perror(PTHREAD_ERR_MSG);
	cleanup(&ctx, &t);
	return (0);
}
