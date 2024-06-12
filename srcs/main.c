#include "wirelescanner.h"

static int	init_top_pthreads(t_thread_ids *t, t_state *s)
{
	if (pthread_mutex_init(&s->bt_info.le_data_mutex, NULL) != 0
		|| pthread_mutex_init(&s->bt_info.hci_data_mutex, NULL) != 0
		|| pthread_mutex_init(&s->bt_info.cl_data_mutex, NULL) != 0)
		return (1);
	if (pthread_create(&t->le_scan_id, NULL, le_scan_thread, s) != 0
		|| pthread_create(&t->lst_monitor_id, NULL, lst_monitor_thread, s) != 0
		|| pthread_create(&t->classic_scan_id, NULL, cl_scan_thread, s) != 0)
		//|| pthread_create(&t->le_send_thread, NULL, le_send_thread, s) != 0)
		return (1);
	if (s->ntwrk_info.wifi_scan_on)
	{
		if (pthread_mutex_init(&s->wifi_data_mutex, NULL) != 0
			|| pthread_create(&t->wifi_scan_id, NULL, wifi_scan_thread, s) != 0)
			//|| pthread_create(&t->wifi_send_id, NULL,wifi_senddata, s) != 0)
			return (1);
	}
	return (0);
}

static int	handle_arguments(int ac, char **av, t_all_ntwrk_info *i)
{
	if (ac == 2 && !strcmp(av[1], "wifi"))
		i->wifi_scan_on = true;
	else
		printf(ARG_OPTIONS);
	return (0);
}

static void	join_and_clean(t_state *ctx, t_thread_ids *threads)
{
	pthread_join(threads->le_scan_id, NULL);
	pthread_join(threads->lst_monitor_id, NULL);
	pthread_join(threads->le_send_thread, NULL);
	pthread_mutex_destroy(&ctx->bt_info.le_data_mutex);
	pthread_mutex_destroy(&ctx->bt_info.hci_data_mutex);
	if (ctx->ntwrk_info.wifi_scan_on)
	{
		pthread_join(threads->wifi_send_id, NULL);
		pthread_join(threads->wifi_scan_id, NULL);
		pthread_mutex_destroy(&ctx->wifi_data_mutex);
	}
	clear_lst(ctx, ALL_INFO);
}

void		prepare_all(t_state *state);

int	main(int ac, char **av)
{
	t_state			state;
	t_thread_ids	threads;

	state = (t_state){0};
	threads = (t_thread_ids){0};
	printf("Run as a root !\n");
	state.bleparse = init_ble_parser();
	if (state.bleparse == NULL)
	{
		return (1);
	}
	//print_manufacturer_tree(bleparse->repo.implemented_ad_types.root);
	//print_class_of_device(&bleparse->repo.implemented_ad_types.cod);

	init_signals(&state);
	handle_arguments(ac, av, &state.ntwrk_info);
	prepare_all(&state);
	if (init_top_pthreads(&threads, &state) != 0)
	{
		perror(PTHREAD_ERR_MSG);
	}
	join_and_clean(&state, &threads);
	return (0);
}
