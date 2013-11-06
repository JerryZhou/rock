#include "log.h"

#define LOG_ENABLE_COLOR (1)

#include <time.h>
#ifdef WIN32
#   include <windows.h>
#else
#   include <sys/time.h>
#endif
#ifdef WIN32
int gettimeofday( struct timeval *tv, struct timezone *tz )
{
    time_t rawtime;
    
    time(&rawtime);
    tv->tv_sec = (long)rawtime;
    
    // here starts the microsecond resolution:
    
    LARGE_INTEGER tickPerSecond;
    LARGE_INTEGER tick; // a point in time
    
    // get the high resolution counter's accuracy
    QueryPerformanceFrequency(&tickPerSecond);
    
    // what time is it ?
    QueryPerformanceCounter(&tick);
    
    // and here we get the current microsecond! \o/
    tv->tv_usec = (tick.QuadPart % tickPerSecond.QuadPart);
    
    return 0;
}
#endif

#if LOG_ENABLE_COLOR
#include "colorprint.h"
#define __COLOR_PRINT(color, ...) ColoredPrintf((GTestColor)(color), __VA_ARGS__)
#else
#define __COLOR_PRINT(color, ...) printf(__VA_ARGS__)
#endif

/// colors
#define __sc_color_red      1 //"\033[0;31m"        /* 0 -> normal ;  31 -> red */
#define __sc_color_cyan     6 //"\033[1;36m"        /* 1 -> bold ;  36 -> cyan */
#define __sc_color_green    2 //"\033[4;32m"        /* 4 -> underline ;  32 -> green */
#define __sc_color_blue     4 //"\033[9;34m"        /* 9 -> strike ;  34 -> blue */
#define __sc_color_black    0 //"\033[0;30m"
#define __sc_color_brown    3 //"\033[0;33m"
#define __sc_color_magenta  5 //"\033[0;35m"
#define __sc_color_gray     0 //"\033[0;37m"
#define __sc_color_none     0 //"\033[0m"        /* to flush the previous property */

static int __sc_colors[] = {
    __sc_color_none, // unknown
    __sc_color_none, // default
    __sc_color_none, // verbose
    __sc_color_brown, // debug
    __sc_color_green, // info
    __sc_color_blue, // warn
    __sc_color_red, // error
    __sc_color_cyan, // fatal
    __sc_color_gray, // silent
    __sc_color_black,
    __sc_color_brown,
};

/*
 * Send a simple string to the log.
 **/
int __sc_log_write(int prio, const char *tag, const char *text){
    struct tm *tm;
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    tm=localtime((time_t*)(&tv.tv_sec));
    if (tag) {
        __COLOR_PRINT(__sc_colors[prio], "%d:%02d:%02d %d \t %s \t %s\n",
                      tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec, tag, text);
        return 0;
    }else{
        __COLOR_PRINT(__sc_colors[prio], "%d:%02d:%02d %d %s\n",
                      tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec, text);
        return 0;
    }
}

/*
 * A variant of __android_log_print() that takes a va_list to list
 * additional parameters.
 * */
int __sc_log_vprint(int prio, const char* tag, const char* fmt, va_list ap){
    char content[4096]={0};
    vsnprintf(content, 4095, fmt, ap);
    return __sc_log_write(prio, tag, content);
}

/*
 * Send a formatted string to the log, used like printf(fmt,...)
 **/
int __sc_log_print(int prio, const char* tag, const char* fmt, ...){
    int ret = 0;
    va_list ap;
    va_start(ap, fmt);
    ret = __sc_log_vprint(prio, tag, fmt, ap);
    va_end(ap);
    return ret;
}