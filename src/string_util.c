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
 * Utility module providing string operations.
 */

#include <stdlib.h>
#include <string.h>

#include "string_util.h"

/**
 * Append a character to the provided c string by (re)allocating required memory,
 * and assigning the character value to the new memory cell. Note that this function
 * assumes that the provided c string is null terminated in case it is not null,
 * as it also appends a null terminator character at the end properly.
 *
 * @param str pointer to a proper(null terminated) c string or to a NULL c string.
 * @param c character to append.
 */
extern void
append_char(char ** str, char c) {
    char * b = NULL;

    if (*str == NULL) {
        b = (char *) malloc(2 * sizeof(char));
        b[0] = c;
        b[1] = '\0';
    } else {
      char * d = *str;
      size_t len = strlen(d);
      b = (char *) realloc(*str, len + sizeof(char) + 1);
      b[len] = c;
      b[len + 1] = '\0';
    }
    *str = b;
}

/**
 * Append a character to the provided c string by allocating required memory,
 * and assigning all characters, including the new one to the new memory cells.
 * Note that this function assumes that the provided c string is null terminated
 * in case it is not null, as it also appends a null terminator character at the
 * end properly.
 *
 * @param str proper c string.
 * @param c character to append.
 *
 * @return a proper c string.
 */
extern char *
append_char_to_new_string(char * str, char c) {
    char * b = NULL;

    if (str == NULL) {
        b = (char *) malloc(2 * sizeof(char));
        b[0] = c;
        b[1] = '\0';
    } else {
      size_t len = strlen(str);
      int i;
      b = (char *) malloc(len + sizeof(char) + 1);
      for (i=0; i<len; i++) {
          b[i] = str[i];
      }
      b[len] = c;
      b[len + 1] = '\0';
    }
    return (b);
}

/**
 * Test whether two string values are equal.
 *
 * @param l pointer to the first string value.
 * @param r pointer to the second string value.
 *
 * @return Non-zero if equal, zero otherwise.
 */
extern int
test_string_equal(const void *l, const void *r) {
    int result = 0;
    if (strcmp((char*)l, (char *)r) == 0) {
        result = 1;
    }
    return result;
}
