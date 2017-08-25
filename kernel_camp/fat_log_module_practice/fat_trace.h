#undef TRACE_SYSTEM
#define TRACE_SYSTEM fat_log_practice

#if !defined(_TRACE_FAT_LOG_PRACTICE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_FAT_LOG_PRACTICE_H

#include <linux/tracepoint.h>

/* TODO 
 *  - you need to implement your own tracepoint using TRACE_EVENT macro here.
 */

#endif /* _TRACE_FAT_PRACTICE_H  */


#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#define TRACE_INCLUDE_FILE fat_trace
#include <trace/define_trace.h>
