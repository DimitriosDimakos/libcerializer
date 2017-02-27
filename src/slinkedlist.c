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

#include <stdio.h>
#include <stdlib.h>

#include "stdlib_util.h"
#include "slinkedlist.h"

/**
 * Print on screen the contents (pointers style) of the
 * provided single linked list.
 *
 * @param list single linked list structure.
 */
extern void
slinkedlist_dump(slinkedlist *list) {
    if (!slinkedlist_empty(list)) {
        slinkedlist_node_t *it = list->head;
        while(it) {
            fprintf(stdout, "data reference %p\n", it->data);
            it = it->next;
        }
    }
}

/**
 * Create (and initialize) a single linked list.
 *
 * @return a new single linked list structure.
 */
extern slinkedlist *
slinkedlist_create(void) {
    slinkedlist * sll = (slinkedlist *)SAFE_MALLOC(sizeof(slinkedlist));
    slinkedlist_init(sll);
    return sll;
}

/**
 * Initialize a single linked list.
 *
 * @param list single linked list structure.
 */
extern void
slinkedlist_init(slinkedlist *list) {
    if (list != NULL) {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
    }
}

/**
 * Append data to the single linked list.
 *
 * @param list single linked list structure.
 * @param data data to append.
 */
extern void
slinkedlist_append(slinkedlist *list, void *data) {
    if (list != NULL) {
        slinkedlist_node_t *new_node =
            (slinkedlist_node_t *)SAFE_MALLOC(sizeof(slinkedlist_node_t));
        new_node->data = data;
        new_node->next = NULL;
        if (list->head == NULL) {
            list->head = new_node;
        } else {
            list->tail->next = new_node;
        }
        list->tail = new_node;
        list->size++;
    }
}

/**
 * Prepend data to the single linked list.
 *
 * @param list single linked list structure.
 * @param data data to prepend.
 */
extern void
slinkedlist_prepend(slinkedlist *list, void *data) {
    if (list != NULL) {
        slinkedlist_node_t *new_node =
            (slinkedlist_node_t *)SAFE_MALLOC(sizeof(slinkedlist_node_t));

        new_node->data = data;
        new_node->next = list->head;

        list->head = new_node;
        list->size++;
    }
}

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
slinkedlist_contains(slinkedlist *list, void *data) {
    int result = 0;
    if (!slinkedlist_empty(list)) {
        slinkedlist_node_t *it = list->head;
        while(it) {
            if (it->data == data) {
                result++;
                break;
            }
            it = it->next;
        }
    }
    return result;
}

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
slinkedlist_delete_head(slinkedlist *list) {
    void *data = NULL;
    slinkedlist_node_t *head = NULL;
    if (!slinkedlist_empty(list)) {
        head = list->head; /* save head's address */
        /* mark next link as head */
        list->head = head->next;
        list->size--;
        /* re-adjust tail in case the list became empty */
        if (slinkedlist_empty(list)) {
            list->tail = NULL;
        }
        head->next = NULL;
    }
    if (head == NULL) {
        return NULL;
    } else {
        data = head->data;
        SAFE_FREE(head);
        return data;
    }
}

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
slinkedlist_delete_tail(slinkedlist *list){
    void *data = NULL;
    slinkedlist_node_t *tail = NULL;
    if (!slinkedlist_empty(list)) {
        slinkedlist_node_t *current_node = list->head;
        slinkedlist_node_t *next_node = current_node->next;

        while (next_node != NULL) {
            if (!next_node->next) {
                /* next node is the tail node*/
                break;
            }
            current_node = next_node;
            next_node = current_node->next;
        }
        if (next_node) {
            tail = next_node;
            list->tail = current_node;
            list->tail->next = NULL;
        } else {
            /* list contains only one element and head = tail */
            tail = current_node;
            list->head = NULL;
            list->tail = NULL;
        }
        list->size--;
    }
    if (tail == NULL) {
        return NULL;
    } else {
    	data = tail->data;
    	SAFE_FREE(tail);
        return data;
    }
}

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
slinkedlist_delete_data(slinkedlist *list, void *data) {
    int result = 0;
    if (!slinkedlist_empty(list)) {
        slinkedlist_node_t *previous_node = NULL;
        slinkedlist_node_t *current_node = list->head;
        while (current_node->data != data) {
            if (current_node->next == NULL) {
                return result;
            } else {
                previous_node = current_node;
                current_node = current_node->next;
            }
        }
        result++;/* found a match */
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
        SAFE_FREE(current_node);
        /* decrement structure sizes */
        list->size--;
    }
    return result;
}

/**
 * Reverses all nodes in the single linked list.
 * so that head becomes tail and tail becomes head.
 *
 * @param list single linked list structure.
 */
extern void
slinkedlist_reverse(slinkedlist *list) {
    if (!slinkedlist_empty(list)) {
        slinkedlist_node_t *head = list->head;
        slinkedlist_node_t *prev = NULL;
        slinkedlist_node_t *current = list->head;
        slinkedlist_node_t *next;

        /*
         * iterate the whole linked list structure
         * changing the next node to point to the
         * previous node.
         */
        while (current != NULL) {
            /*   e.g.: 1st iteration
             *  current
             *     |
             *     |
             *     v
             *   ______        ______            ______
             *   |  1  |   |-->|  2  |   |-->    |  n  |   |-->NULL
             *   |-----|   |   |-----|   |       |-----|   |
             *   |_____|---|   |_____|---|   ... |_____|---|
             */
            next = current->next; /* save next node */
            current->next = prev; /* change pointer to the previous node */
            prev = current; /* make previous node the current node */
            current = next; /* make current node the (saved) next node */
            /*   e.g.: after 1st iteration
             *             prev      current
             *              |           |
             *              |           |
             *              v           v
             *            ______     ______                     ______
             * NULL<--|   |  1  |    |  2  |   |-->         |-->|  n  |   |-->NULL
             *        |   |-----|    |-----|   |            |   |-----|   |
             *        |---|_____|    |_____|---|    ...  ---|   |_____|---|
             *
             * e.g.: in the 2nd iteration
             *
             *                            prev      current
             *                             |           |
             *                             |           |
             *                             v           v
             *            ______        ______     ______                      ______
             * NULL<--|   |  1  |<--|   |  2  |    |  3  |   |-->          |-->|  n  |   |-->NULL
             *        |   |-----|   |   |-----|    |-----|   |             |   |-----|   |
             *        |---|_____|   |---|_____|    |_____|---|     ...  ---|   |_____|---|
             *
             * e.t.c
             */
        }

        list->head = prev;
        list->tail = head;
    }
}

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
slinkedlist_to_array(slinkedlist *list) {
    void ** array = NULL;
    if (!slinkedlist_empty(list)) {
        /* array = (void **)SAFE_MALLOC(list->size * sizeof(void *));
           (do not use SAFE_MALLOC since it is user responsibility to free) */
        array = (void **)malloc(list->size * sizeof(void *));
        if (array != NULL) {
            int i = 0;
            slinkedlist_node_t *it = list->head;
            while(it) {
                array[i++] = it->data;
                it = it->next;
            }
        }
    }
    return array;
}

/**
 * Tests whether the provided single linked list is empty.
 *
 * @param list single linked list structure to use.
 *
 * @return Non-zero if single linked list is empty, zero otherwise.
 */
extern int
slinkedlist_empty(slinkedlist *list) {
    int result = 0;
    if (list == NULL) {
        result++;
    } else {
        if (list->size == 0) {
            result++;
        }
    }
    return result;
}

/**
 * Free the single linked list and all its nodes and data. Uses
 * slinkedlist_data_dealloc function passed.
 * Note that in case slinkedlist_data_dealloc was not provided,
 * data of each node in the linked list remains intact(not free'd);
 *
 * @param list single linked list structure.
 * @param free_data data de-allocation function.
 */
extern void
slinkedlist_free(slinkedlist *list, slinkedlist_data_dealloc free_data) {
    slinkedlist_node_t *current_node = list->head;
    if (!slinkedlist_empty(list)) {
        slinkedlist_node_t *next_node = current_node->next;

        while (next_node != NULL) {
            if (free_data != NULL) {
                free_data(current_node->data);
            }
            SAFE_FREE(current_node);
            current_node = next_node;
            next_node = current_node->next;
        }

        if (free_data != NULL) {
            free_data(current_node->data);
        }
        SAFE_FREE(current_node);
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}
