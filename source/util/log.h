#ifndef __LOG_H_
#define __LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

enum {
    SC_LOG_UNKNOWN = 0,
    SC_LOG_DEFAULT,    /* only for SetMinPriority() */
    SC_LOG_VERBOSE,
    SC_LOG_DEBUG,
    SC_LOG_INFO,
    SC_LOG_WARN,
    SC_LOG_ERROR,
    SC_LOG_FATAL,
    SC_LOG_SILENT,     /* only for SetMinPriority(); must be last */
};

/*
 * Send a simple string to the log.
 **/
int __sc_log_write(int prio, const char *tag, const char *text);

/*
 * A variant of __android_log_print() that takes a va_list to list
 * additional parameters.
 * */
int __sc_log_vprint(int prio, const char* tag, const char* fmt, va_list ap);

/*
 * Send a formatted string to the log, used like printf(fmt,...)
 **/
int __sc_log_print(int prio, const char* tag, const char* fmt, ...);

#endif