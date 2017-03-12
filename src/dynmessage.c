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
 * Implementation of a generic dynamic message structure.
 */

#include <string.h>
#include <stdio.h>

#include "dynmessage.h"
#include "log.h"
#include "hashmap.h"
#include "slinkedlist.h"
#include "stdlib_util.h"
#include "string_util.h"

/**
 * Function to test whether a dynamic message has been initialized or not.
 *
 * @param message dynamic message structure(not NULL).
 *
 * @return Non zero if the dynamic message has been initialized, zero otherwise.
 */
static int
dynmessage_initialized(dynamicmessage *message) {
    int result = 0;
    if (message != NULL) {
        /* message is not null, verify if hashmap has been created and initialized. */
        hashmap *fields_info = (hashmap *) message->fields_info; /* use field_info as a hashmap */
        if (fields_info != NULL) {
            if (fields_info->table != NULL) {
                result++;
            }
        }
    }
    return result;
}

/**
 * Function to update a field and/or value to a dynamic message.
 *
 * @param message dynamic message structure(not NULL).
 * @param name name of the field(not NULL).
 * @param type type of the field.
 * @param value of the field.
 */
static void
update_field_value(
    dynamicmessage *message, char *name, void *value) {

    hashmap *fields_info = (hashmap *) message->fields_info; /* use field_info as a hashmap */
    dyn_field * field = hashmap_get(fields_info, name);
    dyn_field_value * value_to_store = field->value;

    if (value_to_store == NULL) {
        value_to_store = (dyn_field_value *)SAFE_MALLOC(sizeof(dyn_field_value));
    } else {
        if (field->type == STRING_TYPE) { /* clear it to be replaced with the new one */
            free(value_to_store->string_value);
        }
    }
    /* add the new value */
    switch(field->type) {
    case ENUMERATION_TYPE:
        value_to_store->enum_value = *(unsigned int *)value;
        break;
    case INT8_TYPE:
        value_to_store->int8_value = *(char *)value;
        break;
    case UNSIGNED_INT8_TYPE:
        value_to_store->uint8_value = *(unsigned char *)value;
        break;
    case INT16_TYPE:
        value_to_store->int16_value = *(int *)value;
        break;
    case UNSIGNED_INT16_TYPE:
        value_to_store->uint16_value = *(unsigned int *)value;
        break;
    case INT32_TYPE:
        value_to_store->int32_value = *(long *)value;
        break;
    case UNSIGNED_INT32_TYPE:
        value_to_store->uint32_value = *(unsigned long *)value;
        break;
    case INT64_TYPE:
        value_to_store->int64_value = *(long long *)value;
        break;
    case UNSIGNED_INT64_TYPE:
        value_to_store->uint64_value = *(unsigned long long *)value;
        break;
    case FLOAT32_TYPE:
        value_to_store->float32_value = *(float *)value;
        break;
    case FLOAT64_TYPE:
        value_to_store->float64_value = *(double *)value;
        break;
    case STRING_TYPE:
        value_to_store->string_value = strdup((char *) value);
        break;
    case NO_TYPE:
        break;
    }

    field->value = value_to_store;
    hashmap_put(fields_info, name, field);
}

/**
 * Allocates memory for the dynamic message structure.
 *
 * @return new dynamic message structure reference.
 */
extern dynamicmessage *
dynmessage_create(void) {
    dynamicmessage * message =
        (dynamicmessage *)SAFE_MALLOC(sizeof(dynamicmessage));
    return message;
}

/**
 * Initialize the dynamic message.
 *
 * @param message dynamic message structure(not NULL).
 * @param name name of the message(not NULL).
 */
extern void
dynmessage_init(dynamicmessage *message, char *name) {
    /* use dynamic message field_info as a hashmap */
    hashmap * field_info;
    if (message != NULL && name != NULL) {
        field_info = (hashmap *)SAFE_MALLOC(sizeof(hashmap));
        message->name = strdup(name);
        hashmap_init(field_info, 17, test_string_equal, NULL);
        message->fields_info = (void *)field_info;
        message->field_count = 0;
    }
}

/**
 * Function to add/update a field and/or value to a dynamic message.
 *
 * @param message dynamic message structure reference(not NULL).
 * @param name name of the field(not NULL).
 * @param type type of the field.
 * @param value of the field(not NULL).
 */
extern void
dynmessage_put_field_and_value(
    dynamicmessage *message,
    char *name,
    dyn_field_type type,
    void *value) {

    hashmap *field_info = NULL;
    /* sanity check */
    if (!dynmessage_initialized(message) || name == NULL || value == NULL) {
        return;
    }

    field_info = (hashmap *) message->fields_info; /* use field_info as a hashmap */
    if (type <ENUMERATION_TYPE || type >STRING_TYPE) {
        return;
    }
    /* check if field is already present */
    if (!hashmap_contains_key(field_info, name)) {
        /* add field to dynamic message */
        dyn_field *field = (dyn_field *) SAFE_MALLOC(sizeof(dyn_field)); /* create dynamic field */
        /* add a new field */
        field->name = strdup(name);
        field->type = type;
        field->value = NULL;
        field->seq = message->field_count+1;
        message->field_count++;
        hashmap_put(field_info, field->name, field);
    }
    if (value != NULL) {
        update_field_value(message, name, value);
    }
}

/**
 * Function to retrieve the value of a dynamic message field.
 *
 * @param message dynamic message structure(not NULL).
 * @param name name of the field(not NULL).
 * @param value of the field(not NULL).
 */
extern void
dynmessage_get_field(
    dynamicmessage *message, char *name, dyn_field *value) {

    hashmap *fields_info = NULL;
    dyn_field *field = NULL;
    /* sanity check */
    if (dynmessage_initialized(message) && name!= NULL) {
        fields_info = message->fields_info;
        field = hashmap_get(fields_info, name);
    }
    /* fill field value with retrieved information */
    if (field == NULL) {
        if (value != NULL) {
            value->name = NULL;
            value->type = NO_TYPE;
            value->value = NULL;
            value->seq = -1;
        }
    } else {
        if (value != NULL) {
            value->name = name;
            value->type = field->type;
            value->value = field->value;
            value->seq = field->seq;
        }
    }
}

/**
 * Return list(dynamic array) of all fields of a dynamic message.
 *
 * @param message dynamic message structure(not NULL).
 *
 * @return list(dynamic array) of all fields of a dynamic message.
 */
extern dyn_field_list *
dynmessage_get_fields(dynamicmessage *message) {
    dyn_field_list * ret = (dyn_field_list *)malloc(sizeof(dyn_field_list));
    if (ret == NULL) {
        log_function_error_message("dynmessage.dynmessage_get_fields",
                "out of memory for dyn_field_list!");
        exit(1);
    } else {
        /* check if there are any fields in the message */
        if (dynmessage_initialized(message) && message->field_count > 0) {
            /* get elements in the list as array */
            dyn_field ** temp_array =
                (dyn_field **)malloc(message->field_count *sizeof(dyn_field *));
            if (temp_array == NULL) {
                log_function_error_message(
                    "dynmessage.dynmessage_get_fields",
                    "out of memory for dyn_field array!");
                exit(1);
            } else {
                /* iterate hash map and add all entries */
                hashmap *fields_info = NULL;
                slinkedlist *field_keys = NULL;
                slinkedlist_node_t *field_keys_it = NULL;

                fields_info = message->fields_info;
                field_keys = hashmap_keys(fields_info);
                field_keys_it = field_keys->head;

                while (field_keys_it) {
                    char *name = (char *) field_keys_it->data;
                    dyn_field *field =(dyn_field *)malloc(sizeof(dyn_field));
                    if (field == NULL) {
                        log_function_error_message(
                            "dynmessage.dynmessage_get_fields",
                            "out of memory for dyn_field!");
                        exit(1);
                    }
                    dynmessage_get_field(message, name, field);
                    temp_array[field->seq-1] = field;

                    field_keys_it = field_keys_it->next;
                }
                slinkedlist_free(field_keys, NULL);
                SAFE_FREE(field_keys);

                ret->list = temp_array;
                ret->list_length = message->field_count;
            }
        } else { /* empty list */
            ret->list_length = 0;
        }
    }

    return ret;
}

/**
 * Free the allocated memory for dynamic message contents.
 *
 * @param message dynamic message structure reference(not NULL).
 */
extern void
dynmessage_free(dynamicmessage *message) {
    hashmap *fields_info = NULL;
    slinkedlist *field_keys = NULL;
    slinkedlist_node_t *field_keys_it = NULL;

    /* sanity check */
    if (dynmessage_initialized(message)) {
        fields_info = message->fields_info;
        field_keys = hashmap_keys(fields_info);
    } else {
        return;
    }
    /* get all keys */
    if (!slinkedlist_empty(field_keys)) {
        field_keys_it = field_keys->head;
    }

    /* remove all elements of field_info, field_values
     *  one by one freeing related allocated memory */
    while (field_keys_it) {
        char *name = (char *) field_keys_it->data;
        hashmap_entry * entry = (hashmap_entry *) hashmap_remove(fields_info, name);

        if (entry != NULL) {
            dyn_field * field = (dyn_field *)entry->value;
            /* free related info hashmap entry */
            free(field->name);
            if (field->type == STRING_TYPE) {
                free(field->value->string_value);
            }
            SAFE_FREE(field->value);
            SAFE_FREE(field);
            SAFE_FREE(entry);
        }

        field_keys_it = field_keys_it->next;
    }
    if (!slinkedlist_empty(field_keys)) {
        slinkedlist_free(field_keys, NULL);
        SAFE_FREE(field_keys);
    }
    hashmap_free(fields_info);
    free(message->name);
    message->field_count = 0;
}

/**
 * Free the allocated memory for dynamic message structure reference.
 *
 * @param message dynamic message structure reference(not NULL).
 */
extern void
dynmessage_destroy(dynamicmessage *message) {
    dynmessage_free(message);
    if (message != NULL) {
        SAFE_FREE(message);
    }
}
