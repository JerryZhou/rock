#include "log.h"

#define LOG_ENABLE_COLOR (1)

#include <time.h>
#ifdef WIN32
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

#ifdef WIN32
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
struct timezone
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME ft;
    unsigned __int64 tmpres = 0;
    static int tzflag;
    
    if (NULL != tv)
    {
        GetSystemTimeAsFileTime(&ft);
        
        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;
        
        /*converting file time to unix epoch*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tmpres /= 10;  /*convert into microseconds*/
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }
    
    if (NULL != tz)
    {
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }
    
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
    time_t timep;
    
    gettimeofday(&tv, NULL);
    time(&timep);
    tm=gmtime(&timep);
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