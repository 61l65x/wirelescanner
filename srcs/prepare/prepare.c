#include "wirelescanner.h"

int			init_bluetooth_ifaces(t_all_bt_info *i);
int			init_ntwrk_ifaces(t_all_ntwrk_info *i);

static void	prepare_interfaces(t_state *state)
{
	if (init_bluetooth_ifaces(&state->bt_info) != 0)
	{
		free_exit(BT_HCI_ERR_MSG, state);
	}
	if (state->ntwrk_info.wifi_scan_on
		&& init_ntwrk_ifaces(&state->ntwrk_info) != 0)
	{
		free_exit(NTWRK_IFACE_ERR_MSG, state);
	}
}

void	prepare_all(t_state *state)
{
	prepare_interfaces(state);
}
