
#include "mainheader.h"

int	get_active_network_interface(char *buffer, size_t buffer_size)
{
	char	*iface_end;
	char	*iface_name_start;
	char	*iface_start;
	size_t	iface_name_length;
	FILE	*fp;
	char	temp_buffer[500];

	iface_end = NULL;
	iface_name_start = NULL;
	iface_start = NULL;
	iface_name_length = 0;
	fp = popen("ip route get 1", "r");
	if (fp == NULL)
	{
		perror("popen");
		return (-1);
	}
	if (fgets(temp_buffer, sizeof(temp_buffer), fp) != NULL)
	{
		iface_start = strstr(temp_buffer, "dev");
		if (iface_start != NULL)
		{
			iface_name_start = iface_start + 4;
			iface_end = strchr(iface_name_start, ' ');
			if (iface_end != NULL)
			{
				*iface_end = '\0';
				iface_name_length = iface_end - iface_name_start;
				if (iface_name_length < buffer_size)
				{
					strncpy(buffer, iface_name_start, buffer_size);
					buffer[buffer_size - 1] = '\0';
					pclose(fp);
					return (0);
				}
			}
		}
	}
	pclose(fp);
	return (-1);
}

int	get_wireless_interfaces(t_ntwrk_iface **ifaces_head)
{
	FILE			*fp;
	char			line[256];
	t_ntwrk_iface	*current_interface;
	t_ntwrk_iface	*last_interface;
	int				count;
	t_ntwrk_iface	*new_iface;

	if ((fp = popen("iw dev", "r")) == NULL)
		return (perror("popen failure"), -1);
	current_interface = NULL;
	last_interface = NULL;
	count = 0;
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		if (strstr(line, "Interface"))
		{
			if ((new_iface = calloc(1, sizeof(t_ntwrk_iface))) == NULL)
			{
				perror("malloc failure");
				continue ;
			}
			sscanf(line, " Interface %s", new_iface->iface_name);
			if (last_interface != NULL)
				last_interface->next = new_iface;
			else
				*ifaces_head = new_iface;
			last_interface = new_iface;
			count++;
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
	pclose(fp);
	return (count);
}

void	free_interfaces(t_ntwrk_iface *head)
{
	t_ntwrk_iface	*tmp;

	while (head)
	{
		tmp = head;
		head = head->next;
		free(tmp);
	}
}
