#pragma once
#include <rte_compat.h>
#include <rte_per_lcore.h>
#ifdef __cplusplus
extern "C" {
#endif
#define RTE_MAGIC 19820526 /**< Magic number written by the main partition when ready. */

/* Maximum thread_name length. */
#define RTE_MAX_THREAD_NAME_LEN 16
/**
 * A wrap API for syscall gettid.
 *
 * @return
 *   On success, returns the thread ID of calling process.
 *   It is always successful.
 */
int rte_sys_gettid(void);

RTE_DECLARE_PER_LCORE(int, _thread_id);

/**
 * Get system unique thread id.
 *
 * @return
 *   On success, returns the thread ID of calling process.
 *   It is always successful.
 */
static inline int rte_gettid(void)
{
	if (RTE_PER_LCORE(_thread_id) == -1)
		RTE_PER_LCORE(_thread_id) = rte_sys_gettid();
	return RTE_PER_LCORE(_thread_id);
}

/**
 * Initialize the Environment Abstraction Layer (EAL).
 *
 * This function is to be executed on the MAIN lcore only, as soon
 * as possible in the application's main() function.
 * It puts the WORKER lcores in the WAIT state.
 *
 * @param argc
 *   A non-negative value.  If it is greater than 0, the array members
 *   for argv[0] through argv[argc] (non-inclusive) shall contain pointers
 *   to strings.
 * @param argv
 *   An array of strings.  The contents of the array, as well as the strings
 *   which are pointed to by the array, may be modified by this function.
 * @return
 *   - On success, the number of parsed arguments, which is greater or
 *     equal to zero. After the call to rte_eal_init(),
 *     all arguments argv[x] with x < ret may have been modified by this
 *     function call and should not be further interpreted by the
 *     application.  The EAL does not take any ownership of the memory used
 *     for either the argv array, or its members.
 *   - On failure, -1 and rte_errno is set to a value indicating the cause
 *     for failure.  In some instances, the application will need to be
 *     restarted as part of clearing the issue.
 *
 *   Error codes returned via rte_errno:
 *     EACCES indicates a permissions issue.
 *
 *     EAGAIN indicates either a bus or system resource was not available,
 *            setup may be attempted again.
 *
 *     EALREADY indicates that the rte_eal_init function has already been
 *              called, and cannot be called again.
 *
 *     EFAULT indicates the tailq configuration name was not found in
 *            memory configuration.
 *
 *     EINVAL indicates invalid parameters were passed as argv/argc.
 *
 *     ENOMEM indicates failure likely caused by an out-of-memory condition.
 *
 *     ENODEV indicates memory setup issues.
 *
 *     ENOTSUP indicates that the EAL cannot initialize on this system.
 *
 *     EPROTO indicates that the PCI bus is either not present, or is not
 *            readable by the eal.
 *
 *     ENOEXEC indicates that a service core failed to launch successfully.
 */
int rte_eal_init(int argc, char **argv);

/**
 * Clean up the Environment Abstraction Layer (EAL)
 *
 * This function must be called to release any internal resources that EAL has
 * allocated during rte_eal_init(). After this call, no DPDK function calls may
 * be made. It is expected that common usage of this function is to call it
 * just before terminating the process.
 *
 * @return
 *  - 0 Successfully released all internal EAL resources.
 *  - -EFAULT There was an error in releasing all resources.
 */
int rte_eal_cleanup(void);

#ifdef __cplusplus
}
#endif