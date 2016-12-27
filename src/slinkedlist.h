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
 * Implementation of a single linked list.
 */

#ifndef SLINKEDLIST_H_
#define SLINKEDLIST_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * Function to free stored data pointers.
 */
typedef void (*slinkedlist_data_dealloc)(void *data);

/**
 * Linked list node structure.
 */
typedef struct _slinkedlist_node_struct {
    /* Pointer to the next node */
    struct _slinkedlist_node_struct *next;
    /* Pointer to data of the current node */
    void *data;
} slinkedlist_node_t;

/**
 * Linked list structure.
 */
typedef struct {
    /* Pointer to leading node */
    slinkedlist_node_t *head;
    /* Size of the single linked_list */
    size_t size;
    /* Pointer to last node */
    slinkedlist_node_t *tail;
} slinkedlist;

/**
 * Print on screen the contents (pointers style) of the
 * provided single linked list.
 *
 * @param list single linked list structure.
 */
extern void
slinkedlist_dump(slinkedlist *list);

/**
 * Create (and initialize) a single linked list.
 *
 * @return a new single linked list structure.
 */
extern slinkedlist *
slinkedlist_create(void);

/**
 * Initialize a single linked list.
 *
 * @param list single linked list structure.
 */
extern void
slinkedlist_init(slinkedlist *list);

/**
 * Append data to the single linked list.
 *
 * @param list single linked list structure.
 * @param data data to append.
 */
extern void
slinkedlist_append(slinkedlist *list, void *data);

/**
 * Prepend data to the single linked list.
 *
 * @param list single linked list structure.
 * @param data data to prepend.
 */
extern void
slinkedlist_prepend(slinkedlist *list, void *data);

/**
 * Returns Non-zero if the single linked list contains
 * the specified reference.
 *
 * @param list single linked list structure.
 * @param data element whose presence in the single linked
 *        list is to be tested.
 *
 * @return Non-zero if the single linked list contains the
 *         specified reference, zero otherwise.
 */
extern int
slinkedlist_contains(slinkedlist *list, void *data);

/**
 * Deletes the head node of the single linked list.
 * Note that this functions does not free stored
 * data in the head node.
 *
 * @param list single linked list structure.
 *
 * @return pointer to the data stored in the deleted head node
 *         of the single linked list.
 */
extern void *
slinkedlist_delete_head(slinkedlist *list);

/**
 * Deletes the tail node of the single linked list.
 * Note that this functions does not free stored
 * data in the tail node.
 *
 * @param list single linked list structure.
 *
 * @return pointer to the data stored in the deleted tail node
 *         of the single linked list.
 */
extern void *
slinkedlist_delete_tail(slinkedlist *list);

/**
 * Deletes the node that contains the specified reference.
 * Note that this functions does not free the specified reference.
 *
 * @param list single linked list structure.
 * @param data reference to be removed from the single linked list.
 *
 * @return Non-zero if the reference was removed from the linked list,
 *         zero otherwise.
 */
extern int
slinkedlist_delete_data(slinkedlist *list, void *data);

/**
 * Reverses all nodes in the single linked list
 * so that head becomes tail and tail becomes head.
 *
 * @param list single linked list structure.
 */
extern void
slinkedlist_reverse(slinkedlist *list);

/**
 * Return an array containing all data of the provided single linked list.
 *
 * @param list single linked list structure to use.
 *
 * @return array containing all data of the provided single linked list
 *         structure (can be NULL if single linked list is empty or
 *         failed to allocate enough memory).
 */
extern void **
slinkedlist_to_array(slinkedlist *list);

/**
 * Tests whether the provided single linked list is empty.
 *
 * @param list single linked list structure to use.
 *
 * @return Non-zero if single linked list is empty, zero otherwise.
 */
extern int
slinkedlist_empty(slinkedlist *list);

/**
 * Free the single linked list and all its nodes and data. Uses
 * slinkedlist_data_dealloc function passed.
 * Note that in case slinkedlist_data_dealloc was not provided,
 * data of each node in the linked list remains intact(not free'd);
 *
 * @param list single linked list structure.
 * @param free_data data de-allocation function For automatically
 *        allocated memory, pass in `NULL`.
 *        For memory allocated with the standard library `malloc`
 *        function, pass in standard library `free` function.
 */
extern void
slinkedlist_free(slinkedlist *list, slinkedlist_data_dealloc free_data);

#ifdef  __cplusplus
}
#endif

#endif /* SLINKEDLIST_H_ */
