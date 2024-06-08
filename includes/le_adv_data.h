#ifndef LE_ADV_DATA_H
# define LE_ADV_DATA_H

# include <stdint.h>
# include <yaml.h>

// Top of the structure hierarchy for the LE advertising data repository
typedef struct s_ad_types
{
	uint8_t				type;
	char				*name;
	struct s_ad_types	*next;
}						t_ad_types;

typedef struct s_le_adv_data_repository
{
	t_ad_types			*ad_types;
	int					num_le_adv_data;
}						t_le_adv_data_repository;

#endif
