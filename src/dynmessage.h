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

#ifndef DYNMESSAGE_H_
#define DYNMESSAGE_H_

#ifdef  __cplusplus
extern "C" {
#endif

/* Useful macros */
#define dynmessage_put_field(message, name, type) dynmessage_put_field_and_value(message, name, type, NULL)
#define dynmessage_put_enum_field_value(message, name, value) dynmessage_put_field_and_value(message, name, ENUMERATION_TYPE, value)
#define dynmessage_put_int8_field_value(message, name, value) dynmessage_put_field_and_value(message, name, INT8_TYPE, value)
#define dynmessage_put_uint8_field_value(message, name, value) dynmessage_put_field_and_value(message, name, UNSIGNED_INT8_TYPE, value)
#define dynmessage_put_int16_field_value(message, name, value) dynmessage_put_field_and_value(message, name, INT16_TYPE, value)
#define dynmessage_put_uint16_field_value(message, name, value) dynmessage_put_field_and_value(message, name, UNSIGNED_INT16_TYPE, value)
#define dynmessage_put_int32_field_value(message, name, value) dynmessage_put_field_and_value(message, name, INT32_TYPE, value)
#define dynmessage_put_uint32_field_value(message, name, value) dynmessage_put_field_and_value(message, name, UNSIGNED_INT32_TYPE, value)
#define dynmessage_put_int64_field_value(message, name, value) dynmessage_put_field_and_value(message, name, INT64_TYPE, value)
#define dynmessage_put_uint64_field_value(message, name, value) dynmessage_put_field_and_value(message, name, UNSIGNED_INT64_TYPE, value)
#define dynmessage_put_float32_field_value(message, name, value) dynmessage_put_field_and_value(message, name, FLOAT32_TYPE, value)
#define dynmessage_put_float64_field_value(message, name, value) dynmessage_put_field_and_value(message, name, FLOAT64_TYPE, value)
#define dynmessage_put_string_field_value(message, name, value) dynmessage_put_field_and_value(message, name, STRING_TYPE, value)

#define DYN_FIELD_TYPE_LEN 13

/* Enumeration that describes the available types of a dynamic message field.*/
typedef enum _dyn_field_type {
    ENUMERATION_TYPE,    /* use unsigned int */
    INT8_TYPE,           /* use char */
    UNSIGNED_INT8_TYPE,  /* use unsigned char */
    INT16_TYPE,          /* use int */
    UNSIGNED_INT16_TYPE, /* use unsigned int */
    INT32_TYPE,          /* use int, long (depending on system) */
    UNSIGNED_INT32_TYPE, /* use long, long long (depending on system) */
    INT64_TYPE,          /* use long, long long (depending on system) */
    UNSIGNED_INT64_TYPE, /* use unsigned long, unsigned long long (depending on system)  */
    FLOAT32_TYPE,        /* use float */
    FLOAT64_TYPE,        /* use double */
    STRING_TYPE,         /* use char * */
    NO_TYPE              /* do not use */
} dyn_field_type;

/* Union to store dynamic message field value */
typedef union _dyn_field_value_union {
    unsigned int enum_value;
    char int8_value;
    unsigned char uint8_value;
    int int16_value;
    unsigned int uint16_value;
    long int32_value;
    unsigned long uint32_value;
    long long int64_value;
    unsigned long long uint64_value;
    float float32_value;
    double float64_value;
    char * string_value;
} dyn_field_value;

/* Structure to hold dynamic message field information. */
typedef struct _dyn_field_struct {
    char *name; /* name of field */
    dyn_field_type type; /* type of field */
    dyn_field_value  *value; /* field value */
    int seq; /* dynamic field sequence order */
} dyn_field;

/* Structure to hold list (array) of all fields of a dynamic message. */
typedef struct _dyn_field_list {
    dyn_field **list; /* list of field */
    int list_length; /* list length */
} dyn_field_list;

/* Structure to hold dynamic message information. */
typedef struct _dynamicmessage_struct {
    char *name; /* name of message */
    void *fields_info; /* dynamic field information */
    int field_count; /* number of dynamic fields present */
} dynamicmessage;

/**
 * Allocates memory for the dynamic message structure.
 *
 * @return new dynamic message structure reference.
 */
extern dynamicmessage *
dynmessage_create(void);

/**
 * Initialize the dynamic message.
 *
 * @param message dynamic message structure(not NULL).
 * @param name name of the message(not NULL).
 */
extern void
dynmessage_init(dynamicmessage *message, char *name);

/**
 * Function to add/update a field and/or value to a dynamic message.
 *
 * @param message dynamic message structure reference(not NULL).
 * @param name name of the field(not NULL).
 * @param type type of the field.
 * @param value of the field.
 */
extern void
dynmessage_put_field_and_value(
    dynamicmessage *message,
    char *name,
    dyn_field_type type,
    void *value);

/**
 * Function to retrieve the value of a dynamic message field.
 *
 * @param message dynamic message structure reference(not NULL).
 * @param name name of the field(not NULL).
 * @param value of the field(not NULL).
 */
extern void
dynmessage_get_field(
    dynamicmessage *message, char *name, dyn_field *value);

/**
 * Return list(dynamic array) of all fields of a dynamic message.
 *
 * @param message dynamic message structure reference(not NULL).
 *
 * @return list(dynamic array) of all fields of a dynamic message.
 */
extern dyn_field_list *
dynmessage_get_fields(dynamicmessage *message);

/**
 * Free the allocated memory for dynamic message contents.
 *
 * @param message dynamic message structure reference(not NULL).
 */
extern void
dynmessage_free(dynamicmessage *message);

/**
 * Free the allocated memory for dynamic message structure reference,
 * including dynamic message contents.
 *
 * @param message dynamic message structure reference(not NULL).
 */
extern void
dynmessage_destroy(dynamicmessage *message);

#ifdef  __cplusplus
}
#endif

#endif /* DYNMESSAGE_H_ */
