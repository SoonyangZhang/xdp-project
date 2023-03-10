/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2020 Mellanox Technologies, Ltd
 */
#include <string.h>
#include "eal_private.h"
/* Address of global and public configuration */
static struct rte_config rte_config ;
/* Return a pointer to the configuration structure */
struct rte_config *
rte_eal_get_configuration(void)
{
	return &rte_config;
}