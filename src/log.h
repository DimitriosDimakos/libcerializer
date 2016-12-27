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

/**
 * Simple logging module with log control to log statements on screen.
 */

#ifndef LOG_H_
#define LOG_H_

#ifdef    __cplusplus
extern "C" {
#endif

/**
 * The log_level_e enumeration defines a set of standard logging levels
 * that can be used to control logging output.  The logging Level objects
 * are ordered and are specified by ordered integers.
 */
typedef enum {
    OFF_LOG_LEVEL,
    ALL_LOG_LEVELS,
    DEBUG_LOG_LEVEL,
    INFO_LOG_LEVEL,
    WARNING_LOG_LEVEL,
    ERROR_LOG_LEVEL
} log_level_e;

/**
 * Log a message.
 *
 * @param log_level log level to use.
 * @param message the message to log.
 */
extern void
log_message(log_level_e log_level, const char *message);

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
    const char *message);

/**
 * Log a number of elements with the provided output format.
 *
 * @param log_level log level to use.
 * @param format the output format to use.
 * @param ... elements to log
 */
extern void
log_format(log_level_e log_level, const char *format, ...);

/**
 * Log a DEBUG message.
 *
 * @param message the message to log.
 */
extern void
log_debug_message(const char *message);

/**
 * Log a DEBUG message originating from a function.
 *
 * @param func_name the name of the function.
 * @param message the message to log.
 */
extern void
log_function_debug_message(const char *func_name, const char *message);

/**
 * Log a number of DEBUG elements with the provided output format.
 *
 * @param format the output format to use.
 * @param ... elements to log
 */
extern void
log_debug_format(const char *format, ...);

/**
 * Log an INFO message.
 *
 * @param message the message to log.
 */
extern void
log_info_message(const char *message);

/**
 * Log an INFO message originating from a function.
 *
 * @param func_name the name of the function.
 * @param message the message to log.
 */
extern void
log_function_info_message(const char *func_name, const char *message);

/*
 * Log a number of INFO elements with the provided output format.
 *
 * @param format the output format to use.
 * @param ... elements to log
 */
extern void
log_info_format(const char *format, ...);

/**
 * Log a WARNING message.
 *
 * @param message the message to log.
 */
extern void
log_warn_message(const char *message);

/**
 * Log a WARNING message originating from a function.
 *
 * @param func_name the name of the function.
 * @param message the message to log.
 */
extern void
log_function_warn_message(const char *func_name, const char *message);

/**
 * Log a number of WARNING elements with the provided output format.
 *
 * @param format the output format to use.
 * @param ... elements to log
 */
extern void
log_warn_format(const char *format, ...);

/**
 * Log an ERROR message.
 *
 * @param message the message to log.
 */
extern void
log_error_message(const char *message);

/**
 * Log an ERROR message originating from a function.
 *
 * @param func_name the name of the function.
 * @param message the message to log.
 */
extern void
log_function_error_message(const char *func_name, const char *message);

/**
 * Log a number of ERROR elements with the provided output format.
 *
 * @param format the output format to use.
 * @param ... elements to log
 */
extern void
log_error_format(const char *format, ...);

/**
 * Set the active log level.
 *
 * @param log_level the value to set.
 */
extern void
set_log_level(log_level_e log_level);

/**
 * Enables all log levels.
 */
extern void
enable_all_log_levels(void);

/**
 * Disables all log levels.
 */
extern void
switch_off_all_log(void);

#ifdef    __cplusplus
}
#endif

#endif /* LOG_H_ */
