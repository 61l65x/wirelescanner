#include "wirelescanner.h"

int			le_adv_parse_manufacturers(t_le_manufacturer_data *);
int			le_adv_parse_class_of_device(t_le_class_of_device *);
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

int	le_adv_repo_to_structs(t_state *state)
{
	t_le_ad_types	*ad_types;

	ad_types = &state->le_adv_repo.ad_types;
	if (le_adv_parse_manufacturers(&ad_types->manufacturer_data) != 0)
	{
		free_exit(ERR_PARSE_MANUFACTURER, state);
	}
	if (le_adv_parse_class_of_device(&ad_types->cod) != 0)
	{
		free_exit(ERR_PARSE_CLASS_OF_DEVICE, state);
	}
	return (0);
}

void	prepare_all(t_state *state)
{
	prepare_interfaces(state);
	le_adv_repo_to_structs(state);
}
