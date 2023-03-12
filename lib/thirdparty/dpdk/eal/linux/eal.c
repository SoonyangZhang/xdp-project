#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include <getopt.h>
#include <sys/file.h>
#include <stddef.h>
#include <errno.h>
#include <limits.h>
#include <sys/mman.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <rte_compat.h>
#include <rte_common.h>
#include <rte_debug.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_errno.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_log.h>
#include <rte_random.h>
#include <rte_cycles.h>
#include <rte_string_fns.h>

#include "eal_filesystem.h"
#include "eal_thread.h"
#include "eal_private.h"
/* internal configuration (per-core) */
struct lcore_config lcore_config[RTE_MAX_LCORE];

/* parse a sysfs (or other) file containing one integer value */
int
eal_parse_sysfs_value(const char *filename, unsigned long *val)
{
	FILE *f;
	char buf[BUFSIZ];
	char *end = NULL;

	if ((f = fopen(filename, "r")) == NULL) {
		RTE_LOG(ERR, EAL, "%s(): cannot open sysfs value %s\n",
			__func__, filename);
		return -1;
	}

	if (fgets(buf, sizeof(buf), f) == NULL) {
		RTE_LOG(ERR, EAL, "%s(): cannot read sysfs value %s\n",
			__func__, filename);
		fclose(f);
		return -1;
	}
	*val = strtoul(buf, &end, 0);
	if ((buf[0] == '\0') || (end == NULL) || (*end != '\n')) {
		RTE_LOG(ERR, EAL, "%s(): cannot parse sysfs value %s\n",
				__func__, filename);
		fclose(f);
		return -1;
	}
	fclose(f);
	return 0;
}

static void rte_eal_init_alert(const char *msg)
{
	fprintf(stderr, "EAL: FATAL: %s\n", msg);
}
static int
sync_func(__rte_unused void *arg)
{
	return 0;
}
/* Launch threads, called at application init(). */
int
rte_eal_init(int argc, char **argv)
{
    int i,ret;
    pthread_t thread_id;
	static uint32_t run_once;
	uint32_t has_run = 0;
    char cpuset[RTE_CPU_AFFINITY_STR_LEN];
	char thread_name[RTE_MAX_THREAD_NAME_LEN];
    const struct rte_config *config = rte_eal_get_configuration();
    /* checks if the machine is adequate */
	/*if (!rte_cpu_is_supported()) {
		rte_eal_init_alert("unsupported cpu type.");
		rte_errno = ENOTSUP;
		return -1;
	}*/

	if (!__atomic_compare_exchange_n(&run_once, &has_run, 1, 0,
					__ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
		rte_eal_init_alert("already called initialization.");
		rte_errno = EALREADY;
		return -1;
	}
    
    thread_id = pthread_self();
    
	if (rte_eal_cpu_init() < 0) {
		rte_eal_init_alert("Cannot detect lcores.");
		rte_errno = ENOTSUP;
		return -1;
	}
    if (rte_eal_timer_init() < 0) {
        rte_eal_init_alert("Cannot init HPET or TSC timers");
        rte_errno = ENOTSUP;
        return -1;
    }
	if (pthread_setaffinity_np(pthread_self(), sizeof(rte_cpuset_t),
			&lcore_config[config->main_lcore].cpuset) != 0) {
		rte_eal_init_alert("Cannot set affinity");
		rte_errno = EINVAL;
		return -1;
	}
	__rte_thread_init(config->main_lcore,
		&lcore_config[config->main_lcore].cpuset);

	ret = eal_thread_dump_current_affinity(cpuset, sizeof(cpuset));

	RTE_LOG(DEBUG, EAL, "Main lcore %u is ready (tid=%p;cpuset=[%s%s])\n",
		config->main_lcore, thread_id, cpuset,
		ret == 0 ? "" : "...");
	RTE_LCORE_FOREACH_WORKER(i) {

		/*
		 * create communication pipes between main thread
		 * and children
		 */
		if (pipe(lcore_config[i].pipe_main2worker) < 0)
			rte_panic("Cannot create pipe\n");
		if (pipe(lcore_config[i].pipe_worker2main) < 0)
			rte_panic("Cannot create pipe\n");

		lcore_config[i].state = WAIT;

		/* create a thread for each lcore */
		ret = pthread_create(&lcore_config[i].thread_id, NULL,
				     eal_thread_loop, NULL);
		if (ret != 0)
			rte_panic("Cannot create thread\n");

		/* Set thread_name for aid in debugging. */
		snprintf(thread_name, sizeof(thread_name),
				"lcore-worker-%d", i);
		rte_thread_setname(lcore_config[i].thread_id, thread_name);

		ret = pthread_setaffinity_np(lcore_config[i].thread_id,
			sizeof(rte_cpuset_t), &lcore_config[i].cpuset);
		if (ret != 0)
			rte_panic("Cannot set affinity\n");
	}

	/*
	 * Launch a dummy function on all worker lcores, so that main lcore
	 * knows they are all ready when this function returns.
	 */
	rte_eal_mp_remote_launch(sync_func, NULL, SKIP_MAIN);
	rte_eal_mp_wait_lcore();

    return 0;
}
int
rte_eal_cleanup(void)
{
    return 0;
}