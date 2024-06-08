#ifndef LE_ADV_DATA_H
# define LE_ADV_DATA_H

# include <stdint.h>
# include <yaml.h>

// Structure for Manufacturer Specific Data
typedef struct s_le_manufacturer_data
{
	int								id;
	char							*name;
	struct s_le_manufacturer_data	*left;
	struct s_le_manufacturer_data	*right;
}									t_le_manufacturer_data;

// Structure for Class of Device (CoD)
typedef struct s_le_class_of_device
{
	uint32_t						class_of_device;
	char							*major_class;
	char							*minor_class;
	struct s_le_class_of_device		*next;
}									t_le_class_of_device;

typedef struct s_le_ad_types
{
	uint8_t							type;
	char							name;
	t_le_manufacturer_data			manufacturer_data;
	t_le_class_of_device			cod;
	struct s_le_ad_types			*next;
}									t_le_ad_types;

typedef struct s_le_adv_data_repository
{
	t_le_ad_types					ad_types;
	int								num_le_adv_data;
}									t_le_adv_data_repository;

#endif
