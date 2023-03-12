#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "rte_log.h"
DEFINE_LOG_CATEGORY("rte-log");
struct rte_log_dynamic_type {
	uint32_t loglevel;
};

/** The rte_log structure. */
static struct rte_logs {
	uint32_t type;  /**< Bitfield with enabled logs. */
	uint32_t level; /**< Log level. */
	FILE *file;     /**< Output file set by rte_openlog_stream, or NULL. */
	size_t dynamic_types_len;
	struct rte_log_dynamic_type dynamic_types[RTE_LOGTYPE_MAX];
} rte_logs = {
	.type = UINT32_MAX,
	.level = RTE_LOG_DEBUG,
    .dynamic_types_len=RTE_LOGTYPE_MAX,
};
void rte_log_init(){
    LOG_ENSURE_REGISTER();
}
void rte_log_enable(int type,int level){
    if(type<rte_logs.dynamic_types_len){
        rte_logs.dynamic_types[type].loglevel=level;
    }
}
bool
rte_log_can_log(uint32_t logtype, uint32_t level)
{
	bool ret=false;
    if((logtype<rte_logs.dynamic_types_len)&&
        (rte_logs.dynamic_types[logtype].loglevel&level)){
        ret=true;
    }
    return ret;
}
/*
 * Generates a log message The message will be sent in the stream
 * defined by the previous call to rte_openlog_stream().
 * No need to check level here, done by rte_vlog().
 */
int
rte_log(uint32_t level, uint32_t logtype, const char *file,int line,
        const char *format, ...){
    if(rte_log_can_log(logtype,level)||RTE_LOG_CRIT==level){
        va_list ap;
        int ret;
        va_start(ap, format);
        my_vlog_output(g_log_ptr,level,file,line,format,ap);
        va_end(ap);
    }
    return 0;
}
int rte_vlog(uint32_t level, uint32_t logtype, const char *file,int line,const char *format, va_list ap){
    my_vlog_output(g_log_ptr,level,file,line,format,ap);
}