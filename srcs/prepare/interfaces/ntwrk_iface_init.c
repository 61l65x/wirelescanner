
#include "ntwrk_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int	get_wireless_interfaces(t_all_ntwrk_info *i)
{
	FILE			*fp;
	char			line[256];
	t_ntwrk_iface	*current_interface;
	t_ntwrk_iface	*last_interface;
	t_ntwrk_iface	*new_iface;

	if ((fp = popen("iw dev", "r")) == NULL)
		return (-1);
	current_interface = NULL;
	last_interface = NULL;
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		if (strstr(line, "Interface"))
		{
			if ((new_iface = calloc(1, sizeof(t_ntwrk_iface))) == NULL)
				return (pclose(fp), -1);
			sscanf(line, " Interface %s", new_iface->iface_name);
			if (last_interface != NULL)
				last_interface->next = new_iface;
			else
				i->ntwrk_ifaces = new_iface;
			last_interface = new_iface;
			i->num_ntwrk_ifaces++;
		}
		else if (current_interface && strstr(line, "ifindex"))
			sscanf(line, " ifindex %d", &current_interface->ifindex);
		else if (current_interface && strstr(line, "wdev"))
			sscanf(line, " wdev %s", current_interface->wdev);
		else if (current_interface && strstr(line, "addr"))
			sscanf(line, " addr %s", current_interface->addr);
		else if (current_interface && strstr(line, "ssid"))
			sscanf(line, " ssid %s", current_interface->ssid);
		else if (current_interface && strstr(line, "type"))
			sscanf(line, " type %s", current_interface->type);
		if (strstr(line, "Interface"))
			current_interface = last_interface;
	}
	return (pclose(fp), 0);
}

int	init_ntwrk_ifaces(t_all_ntwrk_info *i)
{
	if (get_wireless_interfaces(i) != 0)
		return (-1);
	for (t_ntwrk_iface *iface = i->ntwrk_ifaces; iface; iface = iface->next)
		printf("Interface: %s\n", iface->iface_name);
	return (0);
}
