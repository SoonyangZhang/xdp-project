#pragma once
#include "my_log.h"
#ifdef __cplusplus
extern "C" {
#endif

#define RTE_LOGTYPE_EAL        0 /**< Log related to eal. */
#define RTE_LOGTYPE_MAX  1

/* Can't use 0, as it gives compiler warnings */
#define RTE_LOG_EMERG    LOG_NONE     /**< System is unusable.               */
#define RTE_LOG_ALERT    LOG_WARN    /**< Action must be taken immediately. */
#define RTE_LOG_CRIT     LOG_FATAL  /**< Critical conditions.              */
#define RTE_LOG_ERR      LOG_ERROR  /**< Error conditions.                 */
#define RTE_LOG_WARNING  LOG_WARN  /**< Warning conditions.               */
#define RTE_LOG_NOTICE   LOG_INFO  /**< Normal but significant condition. */
#define RTE_LOG_INFO     LOG_INFO  /**< Informational.                    */
#define RTE_LOG_DEBUG    LOG_DEBUG  /**< Debug-level messages.*/

#define RTE_LOG(l, t, ...)					\
	 rte_log(RTE_LOG_ ## l,					\
		 RTE_LOGTYPE_## t,__FILE__, __LINE__,# t ": " __VA_ARGS__)
void rte_log_init();
void rte_log_enable(int type,int level);
int rte_log(uint32_t level, uint32_t logtype, const char *file,int line,
        const char *format, ...);
int rte_vlog(uint32_t level, uint32_t logtype,const char *file,int line,const char *format, va_list ap);
#ifdef __cplusplus
}
#endif