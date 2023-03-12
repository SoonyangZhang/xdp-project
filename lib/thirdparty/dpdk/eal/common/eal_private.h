#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <rte_lcore.h>

/**
 * Structure storing internal configuration (per-lcore)
 */
struct lcore_config {
	pthread_t thread_id;       /**< pthread identifier */
	int pipe_main2worker[2];   /**< communication pipe with main */
	int pipe_worker2main[2];   /**< communication pipe with main */

	lcore_function_t * volatile f; /**< function to call */
	void * volatile arg;       /**< argument of function */
	volatile int ret;          /**< return value of function */

	volatile enum rte_lcore_state_t state; /**< lcore state */
	unsigned int socket_id;    /**< physical socket id for this lcore */
	unsigned int core_id;      /**< core number on socket for this lcore */
	int core_index;            /**< relative index, starting from 0 */
	uint8_t core_role;         /**< role of core eg: OFF, RTE, SERVICE */

	rte_cpuset_t cpuset;       /**< cpu set which the lcore affinity to */
};

extern struct lcore_config lcore_config[RTE_MAX_LCORE];
/**
 * Get TSC frequency if the architecture supports.
 *
 * This function is private to the EAL.
 *
 * @return
 *   The number of TSC cycles in one second.
 *   Returns zero if the architecture support is not available.
 */
uint64_t get_tsc_freq_arch(void);

/**
 * Configure timers
 *
 * This function is private to EAL.
 *
 * Mmap memory areas used by HPET (high precision event timer) that will
 * provide our time reference, and configure the TSC frequency also for it
 * to be used as a reference.
 *
 * @return
 *   0 on success, negative on error
 */
int rte_eal_timer_init(void);

/**
 * Fill configuration with number of physical and logical processors
 *
 * This function is private to EAL.
 *
 * Parse /proc/cpuinfo to get the number of physical and logical
 * processors on the machine.
 *
 * @return
 *   0 on success, negative on error
 */
int rte_eal_cpu_init(void);
/**
 * The global RTE configuration structure.
 */
struct rte_config {
	uint32_t main_lcore;         /**< Id of the main lcore */
	uint32_t lcore_count;        /**< Number of available logical cores. */
	uint32_t numa_node_count;    /**< Number of detected NUMA nodes. */
	uint32_t numa_nodes[RTE_MAX_NUMA_NODES]; /**< List of detected NUMA nodes. */
	uint32_t service_lcore_count;/**< Number of available service cores. */
	enum rte_lcore_role_t lcore_role[RTE_MAX_LCORE]; /**< State of cores. */
} __rte_packed;

/**
 * Get the global configuration structure.
 *
 * @return
 *   A pointer to the global configuration structure.
 */
struct rte_config *rte_eal_get_configuration(void);

/**
 * Get cpu core_id.
 *
 * This function is private to the EAL.
 */
unsigned eal_cpu_core_id(unsigned lcore_id);

/**
 * Check if cpu is present.
 *
 * This function is private to the EAL.
 */
int eal_cpu_detected(unsigned lcore_id);

/**
 * Set TSC frequency from precise value or estimation
 *
 * This function is private to the EAL.
 */
void set_tsc_freq(void);


/**
 * Init per-lcore info in current thread.
 *
 * @param lcore_id
 *   identifier of lcore.
 * @param cpuset
 *   CPU affinity for this thread.
 */
void __rte_thread_init(unsigned int lcore_id, rte_cpuset_t *cpuset);

/**
 * Uninitialize per-lcore info for current thread.
 */
void __rte_thread_uninit(void);