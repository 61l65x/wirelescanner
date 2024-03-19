#include "mainheader.h"

//convert bssid to printable hardware mac address
const char *bssid_to_string(const uint8_t bssid[BSSID_LENGTH], char bssid_string[BSSID_STRING_LENGTH])
{
	snprintf(bssid_string, BSSID_STRING_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x",
         bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
	return bssid_string;
}

// Get the name of the active network interface
int get_active_network_interface(char *buffer, size_t buffer_size)
{
	char *iface_end = NULL;
	char *iface_name_start = NULL;
	char *iface_start = NULL;
	size_t iface_name_length = 0;
    FILE *fp = popen("ip route get 1", "r");

	if (fp == NULL)
	{
        perror("popen");
        return -1;
    }

    char temp_buffer[500];
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
                    return 0;
                }
            }
        }
    }

    pclose(fp);
    return -1;
}