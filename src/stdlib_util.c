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

/*
 * Utility module providing a rather safe way wrapping of
 * standard library dynamic memory management functions.
 */

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "stdlib_util.h"

#ifdef TEST
int __malloc_counter = 0;
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
    unsigned long lineno) {
    void *ptr = malloc(size);
    if (!ptr) {
        log_function_error_message("stdlib_util.safe_malloc", "out of memory!");
        if (modulename != NULL && funcname != NULL && lineno > 0) {
            log_error_format("[safe_malloc stacktrace] %s:%s:%u\n", modulename, funcname, lineno);
        }
        exit(1);
    }

#ifdef TEST
    __malloc_counter++;
    fprintf(stdout, "safe_malloc: %p, counter: %d\n", ptr, __malloc_counter);
//    if (modulename != NULL && funcname != NULL && lineno > 0) {
//        fprintf(stdout, "[safe_malloc stacktrace] %s:%s:%lu\n", modulename, funcname, lineno);
//    }
#endif

    return ptr;
}

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
    unsigned long lineno) {
    void *new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        log_function_error_message("stdlib_util.safe_realloc", "out of memory!");
        if (modulename != NULL && funcname != NULL && lineno > 0) {
            log_error_format("[safe_realloc stacktrace] %s:%s:%u\n", modulename, funcname, lineno);
        }
        exit(1);
    }
    return new_ptr;
}

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
    unsigned long lineno) {
#ifdef TEST
    fprintf(stdout, "free: %p,", ptr);
#endif

    free(ptr);

#ifdef TEST
    __malloc_counter--;
    fprintf(stdout, "counter: %d\n", __malloc_counter);
//    if (modulename != NULL && funcname != NULL && lineno > 0) {
//      fprintf(stdout, "[safe_free stacktrace] %s:%s:%lu\n", modulename, funcname, lineno);
//    }
#endif
}
