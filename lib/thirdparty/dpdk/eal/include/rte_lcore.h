/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _RTE_LCORE_H_
#define _RTE_LCORE_H_

/**
 * @file
 *
 * API for lcore and socket manipulation
 *
 */
#include "rte_build_config.h"
#include <rte_per_lcore.h>
#include <rte_common.h>
#include <rte_eal.h>
#include <rte_launch.h>
#include <rte_thread.h>
#ifdef __cplusplus
extern "C" {
#endif

#define LCORE_ID_ANY     UINT32_MAX       /**< Any lcore. */

RTE_DECLARE_PER_LCORE(unsigned, _lcore_id);  /**< Per thread "lcore id". */

/**
 * The lcore role (used in RTE or not).
 */
enum rte_lcore_role_t {
	ROLE_RTE,
	ROLE_OFF,
	ROLE_SERVICE,
	ROLE_NON_EAL,
};
/**
 * Get a lcore's role.
 *
 * @param lcore_id
 *   The identifier of the lcore, which MUST be between 0 and RTE_MAX_LCORE-1.
 * @return
 *   The role of the lcore.
 */
enum rte_lcore_role_t rte_eal_lcore_role(unsigned int lcore_id);

/**
 * Test if the core supplied has a specific role
 *
 * @param lcore_id
 *   The identifier of the lcore, which MUST be between 0 and
 *   RTE_MAX_LCORE-1.
 * @param role
 *   The role to be checked against.
 * @return
 *   Boolean value: positive if test is true; otherwise returns 0.
 */
int
rte_lcore_has_role(unsigned int lcore_id, enum rte_lcore_role_t role);

/**
 * Return the Application thread ID of the execution unit.
 *
 * Note: in most cases the lcore id returned here will also correspond
 *   to the processor id of the CPU on which the thread is pinned, this
 *   will not be the case if the user has explicitly changed the thread to
 *   core affinities using --lcores EAL argument e.g. --lcores '(0-3)@10'
 *   to run threads with lcore IDs 0, 1, 2 and 3 on physical core 10..
 *
 * @return
 *  Logical core ID (in EAL thread or registered non-EAL thread) or
 *  LCORE_ID_ANY (in unregistered non-EAL thread)
 */
static inline unsigned
rte_lcore_id(void)
{
	return RTE_PER_LCORE(_lcore_id);
}

/**
 * Get the id of the main lcore
 *
 * @return
 *   the id of the main lcore
 */
unsigned int rte_get_main_lcore(void);

/**
 * Deprecated function the id of the main lcore
 *
 * @return
 *   the id of the main lcore
 */
__rte_deprecated
static inline unsigned int rte_get_master_lcore(void)
{
	return rte_get_main_lcore();
}

/**
 * Return the number of execution units (lcores) on the system.
 *
 * @return
 *   the number of execution units (lcores) on the system.
 */
unsigned int rte_lcore_count(void);

/**
 * Return the index of the lcore starting from zero.
 *
 * When option -c or -l is given, the index corresponds
 * to the order in the list.
 * For example:
 * -c 0x30, lcore 4 has index 0, and 5 has index 1.
 * -l 22,18 lcore 22 has index 0, and 18 has index 1.
 *
 * @param lcore_id
 *   The targeted lcore, or -1 for the current one.
 * @return
 *   The relative index, or -1 if not enabled.
 */
int rte_lcore_index(int lcore_id);

/**
 * Test if an lcore is enabled.
 *
 * @param lcore_id
 *   The identifier of the lcore, which MUST be between 0 and
 *   RTE_MAX_LCORE-1.
 * @return
 *   True if the given lcore is enabled; false otherwise.
 */
int rte_lcore_is_enabled(unsigned int lcore_id);

/**
 * Get the next enabled lcore ID.
 *
 * @param i
 *   The current lcore (reference).
 * @param skip_main
 *   If true, do not return the ID of the main lcore.
 * @param wrap
 *   If true, go back to 0 when RTE_MAX_LCORE is reached; otherwise,
 *   return RTE_MAX_LCORE.
 * @return
 *   The next lcore_id or RTE_MAX_LCORE if not found.
 */
unsigned int rte_get_next_lcore(unsigned int i, int skip_main, int wrap);

/**
 * Macro to browse all running lcores.
 */
#define RTE_LCORE_FOREACH(i)						\
	for (i = rte_get_next_lcore(-1, 0, 0);				\
	     i < RTE_MAX_LCORE;						\
	     i = rte_get_next_lcore(i, 0, 0))

/**
 * Macro to browse all running lcores except the main lcore.
 */
#define RTE_LCORE_FOREACH_WORKER(i)					\
	for (i = rte_get_next_lcore(-1, 1, 0);				\
	     i < RTE_MAX_LCORE;						\
	     i = rte_get_next_lcore(i, 1, 0))

#define RTE_LCORE_FOREACH_SLAVE(l)					\
	RTE_DEPRECATED(RTE_LCORE_FOREACH_SLAVE) RTE_LCORE_FOREACH_WORKER(l)
/**
 * Set thread names.
 *
 * @note It fails with glibc < 2.12.
 *
 * @param id
 *   Thread id.
 * @param name
 *   Thread name to set.
 * @return
 *   On success, return 0; otherwise return a negative value.
 */
int rte_thread_setname(pthread_t id, const char *name);

/**
 * Get thread name.
 *
 * @note It fails with glibc < 2.12.
 *
 * @param id
 *   Thread id.
 * @param name
 *   Thread name to set.
 * @param len
 *   Thread name buffer length.
 * @return
 *   On success, return 0; otherwise return a negative value.
 */
__rte_experimental
int rte_thread_getname(pthread_t id, char *name, size_t len);
#ifdef __cplusplus
}
#endif


#endif /* _RTE_LCORE_H_ */
