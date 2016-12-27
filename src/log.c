/*
 * Copyright 2016 Dimitrios Dimakos
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#ifdef HAVE_FTIME
#define __USE_FTIME
#endif /* HAVE_FTIME */
#endif /* HAVE_CONFIG_H*/

#ifdef __USE_FTIME
#include <sys/timeb.h>
#endif /* __USE_FTIME */

#include "log.h"

#define DEF_BUFFER_LEN 1024

/* log level text */
static const char *LOG_LEVELS_TEXT[] = {
    "[OFF]", "[ALL]", "[DEBUG]", "[INFO]", "[WARNING]", "[ERROR]" };

/* current log level */
static log_level_e level = WARNING_LOG_LEVEL;

/**
 * Tests whether the provided log level is enabled.
 *
 * @param log_level_request requested log level.
 *
 * @return Non-zero if logging is enabled, zero otherwise.
 */
static int
log_level_enabled(log_level_e log_level_request) {
    int result = 0;
    if (level == ALL_LOG_LEVELS || log_level_request == ALL_LOG_LEVELS) {
        result++;
    } else if (log_level_request >= level && level != OFF_LOG_LEVEL) {
        result++;
    }
    return result;
}

/**
 * Log a variable argument list with the provided output format.
 *
 * @param log_level log level to use.
 * @param format the output format to use.
 * @param args variable argument list to log.
 */
static void
log_format_va_list(log_level_e log_level, const char *format, va_list args) {
    if (log_level_enabled(log_level)) {
#ifdef __USE_FTIME
        struct timeb timebuffer;
#else
        time_t time_now = time(NULL);
#endif /* __USE_FTIME */
        char * t;
        FILE * file_p = stdout;
        char log_data[DEF_BUFFER_LEN];
#ifdef __USE_FTIME
        ftime(&timebuffer);
        t = ctime(&(timebuffer.time));
#else
        t = ctime(&time_now);
#endif /* __USE_FTIME */
        t[strlen(t) - 1] = '\0'; /* replace new line character */
        if (log_level >= WARNING_LOG_LEVEL) {
            file_p = stderr;
        }
        vsnprintf(log_data, DEF_BUFFER_LEN, format, args);
#ifdef __USE_FTIME
        fprintf(file_p, "%.19s.%03hu - %s:%s\n", t, timebuffer.millitm,
            LOG_LEVELS_TEXT[log_level], log_data);
#else
        fprintf(file_p, "%s - %s:%s\n", t, LOG_LEVELS_TEXT[log_level], log_data);
#endif /* __USE_FTIME */
    }
}

/**
 * Log a message.
 *
 * @param log_level log level to use.
 * @param message the message to log.
 */
extern void
log_message(log_level_e log_level, const char *message) {
    log_format(log_level, "%s", message);
}

/**
 * Log a message originating from a function.
 *
 * @param log_level log level to use.
 * @param func_name the name of the function.
 * @param message the message to log.
 */
extern void
log_function_message(
    log_level_e log_level,
    const char *func_name,
    const char *message) {

    log_format(log_level, "func:%s:%s", func_name, message);
}

/**
 * Log a number of elements with the provided output format.
 *
 * @param log_level log level to use.
 * @param format the output format to use.
 * @param ... elements to log
 */
extern void
log_format(log_level_e log_level, const char *format, ...) {
    if (log_level_enabled(log_level)) {
#ifdef __USE_FTIME
        struct timeb timebuffer;
#else
        time_t time_now = time(NULL);
#endif /* __USE_FTIME */
        char * t;
        FILE * file_p = stdout;
        va_list args;
        char log_data[DEF_BUFFER_LEN];

#ifdef __USE_FTIME
        ftime(&timebuffer);
        t = ctime(&(timebuffer.time));
#else
        t = ctime(&time_now);
#endif /* __USE_FTIME */
        t[strlen(t) - 1] = '\0'; /* replace new line character */
        if (log_level >= WARNING_LOG_LEVEL) {
            file_p = stderr;
        }
        va_start(args, format);
        vsnprintf(log_data, DEF_BUFFER_LEN, format, args);
        va_end(args);
#ifdef __USE_FTIME
        fprintf(file_p, "%.19s.%03hu - %s:%s\n", t, timebuffer.millitm,
            LOG_LEVELS_TEXT[log_level], log_data);
#else
        fprintf(file_p, "%s - %s:%s\n", t, LOG_LEVELS_TEXT[log_level], log_data);
#endif /* __USE_FTIME */
    }
}

/**
 * Log a DEBUG message.
 *
 * @param message the message to log.
 */
extern void
log_debug_message(const char *message) {
    log_message(DEBUG_LOG_LEVEL, message);
}

/**
 * Log a DEBUG message originating from a function.
 *
 * @param func_name the name of the function.
 * @param message the message to log.
 */
extern void
log_function_debug_message(const char *func_name, const char *message) {
    log_function_message(DEBUG_LOG_LEVEL, func_name, message);
}

/**
 * Log a number of DEBUG elements with the provided output format.
 *
 * @param format the output format to use.
 * @param ... elements to log
 */
extern void
log_debug_format(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_format_va_list(DEBUG_LOG_LEVEL, format, args);
    va_end(args);
}

/**
 * Log an INFO message.
 *
 * @param message the message to log.
 */
extern void
log_info_message(const char *message) {
    log_message(INFO_LOG_LEVEL, message);
}

/**
 * Log an INFO message originating from a function.
 *
 * @param func_name the name of the function.
 * @param message the message to log.
 */
extern void
log_function_info_message(const char *func_name, const char *message) {
    log_function_message(INFO_LOG_LEVEL, func_name, message);
}

/**
 * Log a number of INFO elements with the provided output format.
 *
 * @param format the output format to use.
 * @param ... elements to log.
 */
extern void
log_info_format(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_format_va_list(INFO_LOG_LEVEL, format, args);
    va_end(args);
}

/**
 * Log a WARNING message.
 *
 * @param message the message to log.
 */
extern void
log_warn_message(const char *message) {
    log_message(WARNING_LOG_LEVEL, message);
}

/**
 * Log a WARNING message originating from a function.
 *
 * @param func_name the name of the function.
 * @param message the message to log.
 */
extern void
log_function_warn_message(const char *func_name, const char *message) {
    log_function_message(WARNING_LOG_LEVEL, func_name, message);
}

/**
 * Log a number of WARNING elements with the provided output format.
 *
 * @param format the output format to use.
 * @param ... elements to log
 */
extern void
log_warn_format(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_format_va_list(WARNING_LOG_LEVEL, format, args);
    va_end(args);
}

/**
 * Log an ERROR message.
 *
 * @param message the message to log.
 */
extern void
log_error_message(const char *message) {
    log_message(ERROR_LOG_LEVEL, message);
}

/**
 * Log an ERROR message originating from a function.
 *
 * @param func_name the name of the function.
 * @param message the message to log.
 */
extern void
log_function_error_message(const char *func_name, const char *message) {
    log_function_message(ERROR_LOG_LEVEL, func_name, message);
}

/**
 * Log a number of ERROR elements with the provided output format.
 *
 * @param format the output format to use.
 * @param ... elements to log
 */
extern void
log_error_format(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_format_va_list(ERROR_LOG_LEVEL, format, args);
    va_end(args);
}

/**
 * Set the active log level.
 *
 * @param log_level the value to set.
 */
extern void
set_log_level(log_level_e log_level) {
    level = log_level;
}

/**
 * Enables all log levels.
 */
extern void
enable_all_log_levels(void) {
    set_log_level(ALL_LOG_LEVELS);
}

/**
 * Disables all log levels.
 */
extern void
switch_off_all_log(void) {
    set_log_level(OFF_LOG_LEVEL);
}
