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
 * Utility module providing a rather safe way wrapping of
 * standard library dynamic memory management functions.
 */

#ifndef STDLIB_UTIL_H_
#define STDLIB_UTIL_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stddef.h>

#ifdef USE_SAFE_MEMORY_TRACE
#define SAFE_MALLOC(n) safe_malloc(n, __FILE__, __func__, __LINE__)
#define SAFE_REALLOC(ptr, n) safe_realloc(ptr, n, __FILE__, __func__, __LINE__)
#define SAFE_FREE(ptr) safe_free(ptr, __FILE__, __func__, __LINE__)
#else
#define SAFE_MALLOC(n) safe_malloc(n, NULL, NULL, 0)
#define SAFE_REALLOC(ptr, n) safe_realloc(ptr, n, NULL, NULL, 0)
#define SAFE_FREE(ptr) safe_free(ptr, NULL, NULL, 0)
#endif

#ifdef TEST
extern int __malloc_counter;
#endif

/**
 * Allocate memory while exiting on failure. Also used to count
 * references during testing. All code could call this instead of
 * `malloc`.
 *
 * @param size amount of memory to allocate.
 * @param modulename name of the module which uses safe_malloc.
 * @param funcname name of the function which uses safe_malloc.
 * @param lineno line number of the code which calls safe_malloc.
 *
 * @return pointer to allocated memory.
 */
extern void *
safe_malloc(
    size_t size,
    const char * modulename,
    const char * funcname,
    unsigned long lineno);

/**
 * Re-allocate the previously allocated block in ptr, making
 * the new block SIZE bytes long, while exiting on failure.
 * All code could call this instead of `realloc`.
 *
 * @param ptr pointer to memory to be re-allocated.
 * @param size amount of memory to allocate
 * @param modulename name of the module which uses safe_realloc.
 * @param funcname name of the function which uses safe_realloc.
 * @param lineno line number of the code which calls safe_realloc.
 *
 * @return pointer to re-allocated memory.
 */
extern void *
safe_realloc(
    void *ptr,
    size_t size,
    const char *modulename,
    const char *funcname,
    unsigned long lineno);

/**
 * Pass-through call to free. Also used to count references during
 * testing. All code should call this instead of `free`.
 *
 * @param ptr pointer to de-allocate.
 * @param modulename name of the module which uses safe_free.
 * @param funcname name of the function which uses safe_free.
 * @param lineno line number of the code which calls safe_free.
 */
extern void
safe_free(
    void *ptr,
    const char *modulename,
    const char *funcname,
    unsigned long lineno);

#ifdef  __cplusplus
}
#endif

#endif /* STDLIB_UTIL_H_ */
