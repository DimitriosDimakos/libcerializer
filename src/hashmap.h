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
 * Implementation of a hash map based on a single linked list.
 */

#ifndef HASHMAP_H_
#define HASHMAP_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "slinkedlist.h"

/**
 * Function to determine whether keys `*l` and `*r` are equal.
 *
 * @param l pointer to a key.
 * @param r pointer to another key.
 *
 * @return Non zero if `*l` is equal to `*r`, zero otherwise.
 */
typedef int (*hashmap_key_equal)(const void *l, const void *r);

/**
 * The hash function should calculate a unique hash value
 * for the provided key.
 *
 * @param key key to hash.
 *
 * @return Unique hash value of the key.
 */
typedef size_t (*hashmap_hash_func)(const void *key);

/**
 * Hash map structure
 */
typedef struct {
    /** Maximum size of hash table */
    size_t capacity;
    /** Size of hash table */
    size_t size;
    /** Hash table (use linked list to handle collisions)*/
    slinkedlist **table;
    /** function to compare keys */
    hashmap_key_equal key_equal;
    /** Key hash function */
    hashmap_hash_func hash_func;
} hashmap;

/**
 * Hash map entry (data to be stored in hash table).
 */
typedef struct {
    /** Key */
    void *key;
    /** Value */
    void *value;
} hashmap_entry;

/**
 * Initialize the map.
 *
 * @param map hash map structure.
 * @param capacity initial size of the hash map.
 * @param key_equal function to compare keys.
 * @param hash_func hash function.
 */
extern void
hashmap_init(
    hashmap *map,
    size_t capacity,
    hashmap_key_equal key_equal,
    hashmap_hash_func hash_func);

/**
 * Initialize the map with the default hash function.
 *
 * @param map hash map structure.
 * @param capacity initial size of the hash map.
 * @param key_equal function to compare keys.
 */
extern void
hashmap_init_default(
    hashmap *map,
    size_t capacity,
    hashmap_key_equal key_equal);

/**
 * Free the entire map including the linked lists.
 * Note that data of each entry in the linked list
 * remains intact(not free'd);
 *
 * @param map hash map structure.
 */
extern void
hashmap_free(hashmap *map);

/**
 * Tests whether the provided hash map is empty.
 *
 * @param map hash map structure to use.
 *
 * @return Non-zero if hash map is empty, zero otherwise.
 */
extern int
hashmap_empty(hashmap *map);

/**
 * Removes all entries from the map.
 *
 * @param map hash map structure.
 */
extern void
hashmap_clear(hashmap *map);

/**
 * Get the value for the given key.
 *
 * @param map hash map structure.
 * @param key key for value to fetch.
 *
 * @return pointer to the value.
 */
extern void *
hashmap_get(hashmap *map, void *key);

/**
 * Return a single linked list containing all hashmap_entry objects
 * in the provided map.
 *
 * @param map hash map structure.
 *
 * @return single linked list containing all hashmap_entry objects
 *         in the provided hash map structure
 *         (can be NULL if map is empty or failed to allocate enough memory).
 */
extern slinkedlist *
hashmap_entries(hashmap *map);

/**
 * Return a single linked list containing all keys in the provided map.
 *
 * @param map hash map structure.
 *
 * @return single linked list containing all keys in the provided hash map
 *         structure (can be NULL if map is empty or failed to allocate
 *         enough memory).
 */
extern slinkedlist *
hashmap_keys(hashmap *map);

/**
 * Return a single linked list containing all values in the provided map.
 *
 * @param map hash map structure.
 *
 * @return single linked list containing all values in the provided hash
 *         map structure (can be NULL if map is empty or failed to
 *         allocate enough memory).
 */
extern slinkedlist *
hashmap_values(hashmap *map);

/**
 * Add/update the value into the map.
 *
 * @param map hash map structure.
 * @param key key associated with value.
 * @param value value associated with key.
 */
extern void
hashmap_put(hashmap *map, void *key, void *value);

/**
 * Remove the entry from the map with the provided key.
 *
 * @param map hash map structure.
 * @param key key for mapping to remove.
 *
 * @return pointer to the removed hashmap entry.
 */
extern void *
hashmap_remove(hashmap *map, void *key);

/**
 * Test if the hash map contains the provided key.
 *
 * @param map hash map structure.
 * @param key the key to test.
 *
 * @return Non zero if map contains key, zero otherwise.
 */
extern int
hashmap_contains_key(hashmap *map, void *key);

#ifdef  __cplusplus
}
#endif

#endif /* HASHMAP_H_ */
