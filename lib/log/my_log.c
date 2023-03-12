#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "my_log.h"
#include "../thirdparty/uthash/uthash.h"

#define LOG_BUFFER_SIZE 1024
__thread char g_log_buffer[LOG_BUFFER_SIZE];

typedef struct{
    UT_hash_handle hh;
    const char *name;
    LogLevel level;
    log_sink_cb sink;
}LogLink;

static volatile LogLink *manager=NULL;
LogCategory *create_log(const char *name){
    LogLink *found=NULL;
    HASH_FIND_STR(manager,name,found);
    if(!found){
        found=(LogLink*)malloc(sizeof(*found));
        found->name=name;
        HASH_ADD_KEYPTR(hh,manager,found->name,strlen(found->name),found);
    }
    return (LogCategory*)found;
}
void log_category_enable(const char *name,int level){
    LogLink *found=NULL;
    HASH_FIND_STR(manager,name,found);
    if(found){
        (found->level)|= level;
    }
}
void log_category_disable(const char *name,int level){
    LogLink *found=NULL;
    HASH_FIND_STR(manager,name,found);
    if(found){
        (found->level)|= ~level;
    }
}
void add_custom_log_sink(const char *name,log_sink_cb cb){
    LogLink *found=NULL;
    HASH_FIND_STR(manager,name,found);
    if(found){
        found->sink=cb;
    }
}
bool is_log_enable(LogCategory *log,int level){
    if(log){
        return (level&log->level) ? 1 : 0;
    }
    return false;
}
static const char *level_str(int level){
    switch(level){
    case LOG_FATAL:
        return "fatal";
    case LOG_ERROR:
        return "error";
    case LOG_WARN:
        return "warn";
    case LOG_DEBUG:
        return "debug";
    default:
        return "info";
    }
}
void my_vlog_output(LogCategory *log,int level,const char *file,int line,const char *fmt,va_list ap){
    if(log&&log->sink){
        int l=vsnprintf(g_log_buffer,LOG_BUFFER_SIZE,fmt,ap);
        log->sink(log->name,level,file,line,g_log_buffer,l);
    }else{
        fprintf(stderr,"%s %s:%d ",level_str(level),file,line);
        vfprintf(stderr,fmt,ap);
        fflush(stderr);
    }
}
void my_log_output(LogCategory *log,int level,const char *file,int line,const char *fmt,...){
    if(is_log_enable(log,level)||LOG_FATAL==level){
        va_list args;
        va_start(args,fmt);
        my_vlog_output(log,level,file,line,fmt,args);
        va_end(args);
    }
    if(LOG_FATAL==level){
        abort();
    }
}
