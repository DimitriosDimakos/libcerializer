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
#include <stdlib.h>
#include <string.h>

#include "stdlib_util.h"
#include "slinkedlist.h"

#include "hashmap.h"

/**
 * Default function to test whether two keys are equal
 * @param l first key.
 * @param r second key.
 *
 * @return Non zero if `*l` is equal to `*r`, zero otherwise.
 */
static inline int
hashmap_default_key_equal(const void *l, const void *r) {
    unsigned long result = *((unsigned long *) l) - *((unsigned long *) r);
    if (result == 0) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Hash function provided by Bob Jenkins.
 *
 * @param key key to hash.
 * @param capacity maximum size of the map.
 *
 * @return an offset within the range `[0, capacity)`
 */
static inline size_t
bob_jenkins_hash_func(const void *key, size_t capacity) {
    size_t a = *((size_t *) key);
    a -= (a<<6);
    a ^= (a>>17);
    a -= (a<<9);
    a ^= (a<<4);
    a -= (a<<3);
    a ^= (a<<10);
    a ^= (a>>15);

    return a % capacity;
}

/**
 * The hash function should calculate a unique hash value
 * for the provided key.
 *
 * @param key key to hash.
 *
 * @return Unique hash value of the key.
 */
static inline size_t
default_hash_func(const void *key) {
    size_t a = *((size_t *) key);
    a -= (a<<6);
    a ^= (a>>17);
    a -= (a<<9);
    a ^= (a<<4);
    a -= (a<<3);
    a ^= (a<<10);
    a ^= (a>>15);

    return a;
}

/**
 * Test whether the given hash map requires to be re-hashed,
 * in order to add the provided key.
 *
 * @param map hash map structure.
 * @param key the key.
 *
 * @return Non-zero if map requires re-hash, zero otherwise.
 */
static int
hashmap_rehash_required(hashmap *map, void *key) {
    slinkedlist_node_t *it = NULL;
    size_t offset = map->hash_func(key) % map->capacity;
    size_t new_key_offset;
    size_t present_key_offset;
    slinkedlist *list = map->table[offset];
    int rehash = 0;
    if (list == NULL) {
        return rehash;
    }

    new_key_offset = map->hash_func(key) % (2 * map->capacity);

    it = list->head;
    while (it) {
        hashmap_entry *entry = (hashmap_entry *) it->data;
        /* recalculate offset of present keys */
        present_key_offset = map->hash_func(entry->key) % (2 * map->capacity);
        if ((new_key_offset != present_key_offset) && (map->capacity == map->size)) {
            rehash++;
        }
        it = it->next;
    }
    return rehash;
}

/**
 * Resize and re-hash the provided hash map.
 *
 * @param map hash map structure.
 * @param capacity desired maximum size of the hash map.
 */
static void
hashmap_rehash(hashmap *map, size_t capacity) {
    slinkedlist_node_t * it = NULL;
    /* save all entries in hash map */
    slinkedlist * entries = hashmap_entries(map);
    /* allocate memory for new hash table */
    slinkedlist **table = (slinkedlist **)SAFE_MALLOC(sizeof(slinkedlist *) * capacity);
    memset(table, 0, sizeof(slinkedlist *) * capacity);
    /* clear hash map */
    hashmap_clear(map);
    SAFE_FREE(map->table);
    /* set new table */
    map->table = table;
    /* set new capacity */
    map->capacity = capacity;
    /* add entries in the hash table */
    it = entries->head;
    while(it) {
        hashmap_entry * entry = (hashmap_entry *)it->data;
        hashmap_put(map, entry->key, entry->value);
        it = it->next;
    }
    /* free entries linked list */
    slinkedlist_free(entries, (slinkedlist_data_dealloc)safe_free);
    SAFE_FREE(entries);
}

/**
 * Initialize the map.
 *
 * @param map hash map structure.
 * @param capacity maximum size of the hash map.
 * @param key_equal function to compare keys.
 * @param hash_func hash function.
 */
extern void
hashmap_init(
    hashmap *map,
    size_t capacity,
    hashmap_key_equal key_equal,
    hashmap_hash_func hash_func) {

    if (map != NULL) {
        if ((int)capacity <= 0) {
        } else {
            map->capacity = capacity;
        }
        map->size = 0;

        map->table = (slinkedlist **)SAFE_MALLOC(sizeof(slinkedlist *) * map->capacity);
        memset(map->table, 0, sizeof(slinkedlist *) * map->capacity);

        if (key_equal) {
            map->key_equal = key_equal;
        } else {
            map->key_equal = hashmap_default_key_equal;
        }

        if (hash_func) {
            map->hash_func = hash_func;
        } else {
            map->hash_func = default_hash_func;
        }
    }
}

/**
 * Initialize the map with the default hash function.
 *
 * @param map hash map structure.
 * @param capacity maximum size of the hash map.
 * @param key_equal function to compare keys.
 */
extern void
hashmap_init_default(
    hashmap *map,
    size_t capacity,
    hashmap_key_equal key_equal,
    hashmap_hash_func hash_func) {
    hashmap_init(map, capacity, key_equal, default_hash_func);
}

/**
 * Free the entire map including the linked lists.
 * Note that data of each entry in the linked list
 * remains intact(not free'd);
 *
 * @param map hash map structure.
 * @param free_data data de-allocation function.
 */
extern void
hashmap_free(hashmap *map) {
    if (map != NULL) {
        size_t i;
        for (i = 0; i < map->capacity; i++) {
            if (map->table[i]) {
                slinkedlist_free(map->table[i], (slinkedlist_data_dealloc) safe_free);
                SAFE_FREE(map->table[i]);
            }
        }
        SAFE_FREE(map->table);
        SAFE_FREE(map);
    }
}

/**
 * Tests whether the provided hash map is empty.
 *
 * @param map hash map structure to use.
 *
 * @return Non-zero if hash map is empty, zero otherwise.
 */
extern int
hashmap_empty(hashmap *map) {
    int result = 0;
    if (map == NULL) {
        result++;
    } else {
        if (map->table == NULL) {
            result++;
        } else {
            if (map->size == 0) {
                result++;
            }
        }
    }
    return result;
}

/**
 * Removes all entries from the map.
 *
 * @param map hash map structure.
 */
extern void
hashmap_clear(hashmap *map) {
    size_t i;
    for (i = 0; i < map->capacity; i++) {
        slinkedlist *list = map->table[i];
        if (list) {
            slinkedlist_free(list, (slinkedlist_data_dealloc) safe_free);
            SAFE_FREE(list);
            map->table[i] = NULL;
        }
    }
    map->size = 0;
}

/**
 * Get the value for the given key.
 *
 * @param map hash map structure.
 * @param key key for value to fetch.
 *
 * @return pointer to the value.
 */
extern void *
hashmap_get(hashmap *map, void *key) {
    if (hashmap_empty(map) || key == NULL) {
        return NULL;
    } else {
        slinkedlist *list = map->table[map->hash_func(key) % map->capacity];
        slinkedlist_node_t *it = NULL;
        if (!list) {
            return NULL;
        }
        it = list->head;
        while (it) {
            hashmap_entry *entry = (hashmap_entry *) it->data;
            if (map->key_equal(entry->key, key)) {
                return entry->value;
            }
            it = it->next;
        }
        return NULL;
    }
}

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
hashmap_entries(hashmap *map) {
    slinkedlist * list = NULL;
    if (!hashmap_empty(map)) {
        list = slinkedlist_create();
        if (list != NULL) {
            size_t i;
            for (i=0; i<map->capacity; i++) {
                if (map->table[i]) {
                    slinkedlist_node_t * it = map->table[i]->head;
                    while(it) {
                        hashmap_entry *entry = (hashmap_entry *)SAFE_MALLOC(sizeof(hashmap_entry));
                        hashmap_entry *data = (hashmap_entry *)it->data;
                        entry->key = data->key;
                        entry->value = data->value;
                        slinkedlist_append(list, entry);
                        it = it->next;
                    }
                }
            }
        }
    }
    return list;
}

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
hashmap_keys(hashmap *map){
    slinkedlist * list = NULL;
    if (!hashmap_empty(map)) {
        list = slinkedlist_create();
        if (list != NULL) {
            size_t i;
            for (i=0; i<map->capacity; i++) {
                if (map->table[i]) {
                    slinkedlist_node_t * it = map->table[i]->head;
                    while(it) {
                        hashmap_entry *entry = (hashmap_entry *)it->data;
                        slinkedlist_append(list, entry->key);
                        it = it->next;
                    }
                }
            }
        }
    }
    return list;
}

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
hashmap_values(hashmap *map) {
    slinkedlist * list = NULL;
    if (!hashmap_empty(map)) {
        list = slinkedlist_create();
        if (list != NULL) {
            size_t i;
            for (i=0; i<map->capacity; i++) {
                if (map->table[i]) {
                    slinkedlist_node_t * it = map->table[i]->head;
                    while(it) {
                        hashmap_entry *entry = (hashmap_entry *)it->data;
                        slinkedlist_append(list, entry->value);
                        it = it->next;
                    }
                }
            }
        }
    }
    return list;
}

/**
 * Add/update the value into the map.
 *
 * @param map hash map structure.
 * @param key key associated with value.
 * @param value value associated with key.
 */
extern void
hashmap_put(hashmap *map, void *key, void *value) {
    if (map != NULL && key != NULL) {
        size_t offset = map->hash_func(key) % map->capacity;
        slinkedlist *list =  map->table[offset];;

        if (list == NULL) {
            hashmap_entry *entry = NULL;
            /* create a new linked list */
            list = (slinkedlist *)SAFE_MALLOC(sizeof(slinkedlist));
            slinkedlist_init(list);
            map->table[offset] = list;

            entry = (hashmap_entry *)SAFE_MALLOC(sizeof(hashmap_entry));
            entry->key = key;
            entry->value = value;
            slinkedlist_prepend(list, entry);
            map->size++;
        } else {
            /* key exists or hash collision */
            int found = 0;
            slinkedlist_node_t *it = list->head;
            while (it) {
                hashmap_entry *entry = (hashmap_entry *) it->data;
                if (map->key_equal(entry->key, key)) {
                    /* key already exists, update the value */
                    entry->value = value;
                    found++;
                    break;
                }
                it = it->next;
            }
            if (!found) { /* hash collision or we have reached the maximum table size */
                if (hashmap_rehash_required(map, key)) {
                    hashmap_rehash(map, 2 * map->capacity);
                    hashmap_put(map, key, value);
                } else {
                    /* add a new node in the linked list */
                    hashmap_entry *entry = (hashmap_entry *)SAFE_MALLOC(sizeof(hashmap_entry));
                    entry->key = key;
                    entry->value = value;
                    slinkedlist_prepend(list, entry);
                    map->size++;
                }
            }
        }
    }
}

/**
 * Remove the entry from the map with the provided key.
 *
 * @param map hash map structure.
 * @param key key for mapping to remove.
 *
 * @return pointer to the removed hashmap entry.
 */
extern void *
hashmap_remove(hashmap *map, void *key) {
    if (hashmap_empty(map) || key == NULL) {
        return NULL;
    } else {
        slinkedlist *list = map->table[map->hash_func(key) % map->capacity];
        hashmap_entry *del_entry = NULL;
        hashmap_entry *del_entry_data = NULL;
        slinkedlist_node_t *previous_node = NULL;
        slinkedlist_node_t *current_node = NULL;

        if (!list) {
            return NULL;
        }
        current_node = list->head;
        while (!map->key_equal(((hashmap_entry *) current_node->data)->key, key)) {
            if (current_node->next == NULL) {
                return NULL;
            } else {
                previous_node = current_node;
                current_node = current_node->next;
            }
        }
        /* found a match */
        if (current_node == list->head) {
            /* change first to point to next link */
            list->head = list->head->next;
        } else {
            /* bypass the current link */
            previous_node->next = current_node->next;
        }
        /* re-adjust tail in case it's identical to the deleted node */
        if (current_node == list->tail) {
            list->tail = previous_node;
        }

        del_entry = (hashmap_entry *)SAFE_MALLOC(sizeof(hashmap_entry));
        del_entry_data = (hashmap_entry *)current_node->data;
        del_entry->key = del_entry_data->key;
        del_entry->value = del_entry_data->value;

        SAFE_FREE(current_node->data);
        SAFE_FREE(current_node);
        /* decrement structure sizes */
        list->size--;
        map->size--;
        return del_entry;
    }
}

/**
 * Test if the hash map contains the provided key.
 *
 * @param map hash map structure.
 * @param key the key to test.
 *
 * @return Non zero if map contains key, zero otherwise.
 */
extern int
hashmap_contains_key(hashmap *map, void *key) {
    if (map != NULL && key != NULL) {
        slinkedlist *list = map->table[map->hash_func(key) % map->capacity];
        slinkedlist_node_t *it = NULL;
        if (!list) {
            return 0;
        }
        it = list->head;
        while (it) {
            hashmap_entry *entry = (hashmap_entry *) it->data;
            if (map->key_equal(entry->key, key)) {
                return 1;
            }
            it = it->next;
        }
    }
    return 0;
}
