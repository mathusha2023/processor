#ifndef LOG_H
#define LOG_H

// отключает логи
// #define DISABLE_LOGS

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "config.h"

const size_t MAX_TIME_BUF = 100;

void restart_log();
void _logfunc(const char *__file__, int __line__, int need_console, const char *message, ...);

#ifdef DISABLE_LOGS

#define LOG(message, ...)
#define FLOG(message, ...)

#else

#define _log(need_console, message, ...) _logfunc(__FILE__, __LINE__, need_console, message, ##__VA_ARGS__)
#define LOG(message, ...) _log(1, message, ##__VA_ARGS__)
#define FLOG(message, ...) _log(0, message, ##__VA_ARGS__)

#endif // DISABLE_LOGS

#endif // LOG_H