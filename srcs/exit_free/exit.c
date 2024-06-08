#include "wirelescanner.h"



void free_exit(const char *err_msg, t_state *state)
{
    if (err_msg)
        perror(err_msg);
    clear_lst(state, ALL_INFO);
    if (err_msg)
        exit(EXIT_FAILURE);
    exit(EXIT_SUCCESS);
}
