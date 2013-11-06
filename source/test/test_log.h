#include "util/log.h"

TINYTEST_SUIT(log);

TINYTEST_CASE(log, logLevel)
{
    TINYTEST_TRUE();
    __sc_log_write(0, "test", "information");
}

TINYTEST_CASE(log, allLevels)
{
    TINYTEST_TRUE();
    __sc_log_write(0, "test", "log level 0");
    __sc_log_write(1, "test", "log level 1");
    __sc_log_write(2, "test", "log level 2");
    __sc_log_write(3, "test", "log level 3");
    __sc_log_write(4, "test", "log level 4");
    __sc_log_write(5, "test", "log level 5");
}

TINYTEST_CASE(log, __sc_log_print)
{
    TINYTEST_TRUE();
    __sc_log_print(0, "test", "format %s: level %d", "tag", 0);
    __sc_log_print(1, "test", "format %s: level %d", "tag", 1);
    __sc_log_print(2, "test", "format %s: level %d", "tag", 2);
    __sc_log_print(3, "test", "format %s: level %d", "tag", 3);
    __sc_log_print(4, "test", "format %s: level %d", "tag", 4);
    __sc_log_print(5, "test", "format %s: level %d", "tag", 5);
}