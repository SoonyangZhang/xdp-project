#include <stdio.h>
#include <rte_compat.h>
#include <rte_common.h>
#include <rte_debug.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_errno.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>

#include "eal_private.h"
/* internal configuration (per-core) */
struct lcore_config lcore_config[RTE_MAX_LCORE];

static void rte_eal_init_alert(const char *msg)
{
	fprintf(stderr, "EAL: FATAL: %s\n", msg);
}
/* Launch threads, called at application init(). */
int
rte_eal_init(int argc, char **argv)
{
    if (rte_eal_timer_init() < 0) {
        rte_eal_init_alert("Cannot init HPET or TSC timers");
        rte_errno = ENOTSUP;
        return -1;
    }
    return 0;
}