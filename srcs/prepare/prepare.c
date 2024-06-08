#include "wirelescanner.h"

int			le_adv_repo_to_data_structs(t_le_adv_data_repository *le_data);

static void	prepare_interfaces(t_state *state)
{
    if (init_bluetooth_ifaces(&state->bt_info) != 0)
	{
		perror(BT_HCI_ERR_MSG);
		clear_lst(state, HCI_INFO);
	}
	if (state->ntwrk_info.wifi_scan_on
		&& init_ntwrk_ifaces(&state->ntwrk_info) != 0)
	{
		perror(NTWRK_IFACE_ERR_MSG);
		clear_lst(state, ALL_INFO);
	}
}

void	prepare_all(t_state *state)
{
    prepare_interfaces(state);
}

