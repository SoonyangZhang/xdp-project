#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
/*
reference 
https://github.com/bmanojlovic/log4c
https://github.com/rxi/log.c
https://github.com/nsnam/ns-3-dev-git
*/
typedef enum
{
    LOG_NONE           = 0x00000000, // no logging

    LOG_FATAL          = 0x00000001,
    LOG_LEVEL_FATAL    = 0x00000001,

    LOG_ERROR          = 0x00000002, // serious error messages only
    LOG_LEVEL_ERROR    = 0x00000003,

    LOG_WARN           = 0x00000004, // warning messages
    LOG_LEVEL_WARN     = 0x00000007,

    LOG_DEBUG          = 0x00000008, // rare ad-hoc debug messages
    LOG_LEVEL_DEBUG    = 0x0000000f,

    LOG_INFO           = 0x00000010, // informational messages (e.g., banners)
    LOG_LEVEL_INFO     = 0x0000001f,

    LOG_ALL            = 0x0fffffff, // print everything
    LOG_LEVEL_ALL      = LOG_ALL,
}LogLevel;

typedef void(*log_sink_cb)(const char *name,int level,const char*file,int line,
                            const char *data,int length);
//copy from ut hash
typedef struct UT_hh{
   void *tbl;
   void *prev;                       /* prev element in app order      */
   void *next;                       /* next element in app order      */
   struct UT_hh *hh_prev;   /* previous hh in bucket order    */
   struct UT_hh *hh_next;   /* next hh in bucket order        */
   const void *key;                  /* ptr to enclosing struct's key  */
   unsigned keylen;                  /* enclosing struct's key len     */
   unsigned hashv;                   /* result of hash-fcn(key)        */
}UT_hh;
typedef struct{
    UT_hh hh;
    const char *name;
    LogLevel level;
    log_sink_cb sink;
}LogCategory;
LogCategory *create_log(const char *name);
void log_category_enable(const char *name,int level);
void log_category_disable(const char *name,int level);
void add_custom_log_sink(const char *name,log_sink_cb cb);

#define DEFINE_LOG_CATEGORY(namestr) \
static const char *g_log_name=namestr;\
static LogCategory *g_log_ptr=NULL;

#define LOG_ENSURE_REGISTER() do{\
    g_log_ptr=create_log(g_log_name);\
}while(0)

#define log_trace(...) my_log_output(g_log_ptr,LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) my_log_output(g_log_ptr,LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  my_log_output(g_log_ptr,LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  my_log_output(g_log_ptr,LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) my_log_output(g_log_ptr,LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) my_log_output(g_log_ptr,LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal_check(...) my_log_output(NULL,LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

void my_vlog_output(LogCategory *log,int level,const char *file,int line,const char *fmt,va_list ap);
#define CHECK(x) \
    if(!(x))  log_fatal_check("check failed:" #x);

#ifdef NDEBUG
#define DCHECK(x) \
  while (false) CHECK(x)
#define DCHECK_LT(x, y) \
  while (false) CHECK((x) < (y))
#define DCHECK_GT(x, y) \
  while (false) CHECK((x) > (y))
#define DCHECK_LE(x, y) \
  while (false) CHECK((x) <= (y))
#define DCHECK_GE(x, y) \
  while (false) CHECK((x) >= (y))
#define DCHECK_EQ(x, y) \
  while (false) CHECK((x) == (y))
#define DCHECK_NE(x, y) \
  while (false) CHECK((x) != (y))
#else
#define DCHECK(x) CHECK(x)
#define DCHECK_LT(x, y) CHECK((x) < (y))
#define DCHECK_GT(x, y) CHECK((x) > (y))
#define DCHECK_LE(x, y) CHECK((x) <= (y))
#define DCHECK_GE(x, y) CHECK((x) >= (y))
#define DCHECK_EQ(x, y) CHECK((x) == (y))
#define DCHECK_NE(x, y) CHECK((x) != (y))
#endif  // NDEBUG
void my_log_output(LogCategory *log,int level,const char *file,int line,const char *fmt,...);

