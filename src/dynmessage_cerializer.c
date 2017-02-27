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
 * Generic (de)serializer of a dynamic message.
 */
 
#include <string.h>
#include <stdio.h>

#include "dynmessage_cerializer.h"
#include "dynmessage.h"
#include "stdlib_util.h"
#include "log.h"

#define DYN_MESSAGE_FIXED_LEN 16
#define DYN_FIELD_FIXED_LEN 16
#define DYN_MSG_MIN_LEN 32
#define DYN_MSG_START 1044266557
#define BYTES_4 4
#define BYTES_8 8

/**
 *  SERIALIZED DYNAMIC MESSAGE BINARY FORMAT
 *
 *  dynamic message start                  4 bytes
 *  dynamic message length (total)         4 bytes
 *  dynamic message name length            4 bytes
 *  dynamic message name                   m bytes
 *  dynamic message number of fields (n)   4 bytes
 *
 *  ---> (repeated n times)                n * (16 + k + l) bytes
 *  |   field length (total)               4 bytes
 *  |   field name length                  4 bytes
 *  |   field name                         k bytes
 *  |   field type                         4 bytes
 *  |   field value length                 4 bytes
 *  |   field value                        l bytes
 *  --->
 */

/* sizes to use for serialized field values (fixed for now) */
static const size_t DYN_FIELD_TYPE_SER_SIZE[DYN_FIELD_TYPE_LEN] = {
  4,      /* ENUMERATION_TYPE        */
  2,      /* SHORT_INT_TYPE          */
  2,      /* UNSIGNED_SHORT_INT_TYPE */
  4,      /* INT_TYPE                */
  4,      /* UNSIGNED_INT_TYPE       */
  8,      /* LONG_INT_TYPE           */
  8,      /* UNSIGNED_LONG_INT_TYPE  */
  4,      /* FLOAT_TYPE              */
  8,      /* DOUBLE_TYPE             */
  0,      /* STRING_TYPE             */
  0       /* NO_TYPE                 */
};

/**
 * Function to calculate the length, in bytes, of a serialized dynamic message.
 *
 * @param message reference to the dynamic message object.
 *
 * @return length of the serialized dynamic message object.
 */
static int
calc_dynmessage_serialized_len(dynamicmessage *message) {
    int i;
    int result = 0;
    int field_length = 0;
    /* get a list of all dynamic message fields */
    dyn_field_list * field_list = dynmessage_get_fields(message);
    /* add the size of all dynamic fields */
    if (field_list->list_length > 0) {
        result = DYN_MESSAGE_FIXED_LEN + strlen(message->name);
        for (i=0; i<field_list->list_length;i++) {
            dyn_field *field = field_list->list[i]; /* save field info */
            field_length = DYN_FIELD_FIXED_LEN
                + strlen(field->name)
                + DYN_FIELD_TYPE_SER_SIZE[field->type];
            if (field->type == STRING_TYPE) {
                field_length = field_length + strlen(field->value->string_value);
            }
            result = result + field_length;
            free(field); /* done with this field */
        }
        free(field_list->list); /* done with the field list */
    }
    free(field_list); /* done with the whole list */

    return result;
}

/**
 * Function to verify that a sequence of bytes start with a serialized dynamic message instance.
 *
 * @param data the sequence of bytes representing a serialized dynamic message instance.
 * @param data_len length in bytes of the data byte sequence.
 *
 * @return Non-zero if the sequence of bytes start with a serialized dynamic
 *         message instance, zero otherwise.
 */
static int
verify_dynmessage_start(unsigned char *data, int data_len) {
    int verified = 0;
    /* verify whether provided data has at least the proper length
       for the 'Dynamic Message Start' identifier */
    if (data !=NULL && data_len >= BYTES_4) {
        int dynmessage_start = (int)deserialize_int32(data);
        verified = dynmessage_start == DYN_MSG_START;
    }
    return verified;
}

/**
 * Function to decode the data length in bytes contained in a sequence of bytes
 * representing a serialized dynamic message instance.
 *
 * @param data the sequence of bytes representing a full serialized dynamic message instance.
 * @param data_len length in bytes of the data byte sequence.
 *
 * @return Encoded dynamic message length.
 */
static int
get_encoded_dynmessage_length(unsigned char *data, int data_len) {
    int encoded_dynmessage_length = 0;
    if (data !=NULL && data_len >= BYTES_8) {
        /* use temporary variable to get a slice of the input buffer */
        unsigned char buffer[4];
        strslice(buffer, data, BYTES_4, BYTES_4);
        encoded_dynmessage_length = (int)deserialize_int32(buffer);
    }
    return encoded_dynmessage_length;
}

/**
 * Function to verify that a sequence of bytes contain a full serialized dynamic message instance.
 *
 * @param data the sequence of bytes representing a full serialized dynamic message instance.
 * @param data_len length in bytes of the data byte sequence.
 *
 * @return Non-zero if the sequence of bytes contain a full serialized dynamic message
 *         instance, zero otherwise.
 */
extern int
verify_full_dynmessage(unsigned char *data, int data_len) {
    int verified = 0;
    /* verify that the 'Dynamic Message Start' identifier is present. */
    if (verify_dynmessage_start(data, data_len)) {
        /* verify that all dynamic message data bytes are present. */
        int dynmessage_length = get_encoded_dynmessage_length(data, data_len);
        verified = dynmessage_length <= data_len;
    }
    return verified;
}

/**
 * Function to de-serialize the data of sequence of bytes into a
 * dynamic message structure (binary version).
 *
 * @param data the sequence of bytes representing the data.
 * @param data_len length in bytes of the byte sequence.
 *
 * @return reference to the de-serialized dynamic message structure.
 */
extern void *
dynmessage_deserialize_bin(unsigned char *data, int data_len) {
    size_t start_idx = 0;
    dynamicmessage *dyn_message = NULL;
    if (verify_full_dynmessage(data, data_len)) {
        int i, len, field_count;
        unsigned char buffer4[4];
        unsigned char *message_name = NULL;
        /* skip 'Dynamic Message Start' and dynamic message length bytes */
        start_idx = BYTES_8;
        /* dynamic message name length (4 bytes) */
        strslice(buffer4, data, start_idx, BYTES_4);
        len = (int)deserialize_int32(buffer4);
        start_idx = start_idx + BYTES_4;
        /* dynamic message name (m bytes) */
        message_name =
            (unsigned char *)SAFE_MALLOC((len +1)*sizeof(unsigned char));/* +1 for \0 */
        strslice(message_name, data, start_idx, len);
        message_name[len] = '\0';
        dyn_message = dynmessage_create();
        dynmessage_init(dyn_message, (char *)message_name);
        SAFE_FREE(message_name);
        start_idx = start_idx + len;
        /* dynamic message number of fields (n) (4 bytes) */
        strslice(buffer4, data, start_idx, BYTES_4);
        field_count = (int)deserialize_int32(buffer4);
        start_idx = start_idx + BYTES_4;
        /* de-serialize all dynamic fields */
        if (field_count > 0) {
          for (i = 0; i<field_count; i++) {
              unsigned char *field_name = NULL;
              dyn_field_type field_type;
              unsigned char *field_value_buffer = NULL;
              short short_value;
              int int_value;
              long long_value;
              long long long_long_value;
              unsigned long long ulong_long_value;
              float float_value;
              double double_value;
              /* field length (total) (4 bytes) */
              int field_length;
              strslice(buffer4, data, start_idx, BYTES_4);
              field_length = (int)deserialize_int32(buffer4);
              start_idx = start_idx + BYTES_4;
              /* field name length (4 bytes) */
              strslice(buffer4, data, start_idx, BYTES_4);
              len = (int)deserialize_int32(buffer4);
              start_idx = start_idx + BYTES_4;
              /* field name (k bytes) */
              field_name = 
                  (unsigned char *)SAFE_MALLOC((len +1)*sizeof(unsigned char));/* +1 for \0 */
              strslice(field_name, data, start_idx, len);
              field_name[len] = '\0';
              start_idx = start_idx + len;
              /* field type (4 bytes) */
              strslice(buffer4, data, start_idx, BYTES_4);
              field_type = (int)deserialize_int32(buffer4);
              start_idx = start_idx + BYTES_4;
              /* field value length (4 bytes) */
              strslice(buffer4, data, start_idx, BYTES_4);
              len = (int)deserialize_int32(buffer4);
              dynmessage_put_field(dyn_message, (char*)field_name, field_type);
              start_idx = start_idx + BYTES_4;
              /* field value (l bytes) */
              field_value_buffer =
                  (unsigned char *)SAFE_MALLOC(len*sizeof(unsigned char));
              strslice(field_value_buffer, data, start_idx, len);
              switch(field_type) {
              case ENUMERATION_TYPE: /* 4 bytes */
                  int_value = deserialize_int32(field_value_buffer);
                  dynmessage_put_enum_field_value(dyn_message, (char *)field_name, &int_value);
                  break;
              case INT16_TYPE: /* 2 bytes */
                  short_value = deserialize_int16(field_value_buffer);
                  dynmessage_put_int16_field_value(dyn_message, (char *)field_name, &short_value);
                  break;
              case UNSIGNED_INT16_TYPE: /* 2 bytes */
                  int_value = deserialize_uint16(field_value_buffer);
                  dynmessage_put_uint16_field_value(dyn_message, (char *)field_name, &int_value);
                  break;
              case INT32_TYPE: /* 4 bytes */
                  long_value = deserialize_int32(field_value_buffer);
                  dynmessage_put_int32_field_value(dyn_message, (char *)field_name, &long_value);
                  break;
              case UNSIGNED_INT32_TYPE: /* 4 bytes */
                  long_value = deserialize_uint32(field_value_buffer);
                  dynmessage_put_uint32_field_value(dyn_message, (char *)field_name, &long_value);
                  break;
              case INT64_TYPE: /* 8 bytes */
                  long_long_value = deserialize_int64(field_value_buffer);
                  dynmessage_put_int64_field_value(dyn_message, (char *)field_name, &long_long_value);
                  break;
              case UNSIGNED_INT64_TYPE: /* 8 bytes */
                  ulong_long_value = deserialize_uint64(field_value_buffer);
                  dynmessage_put_uint64_field_value(dyn_message, (char *)field_name, &ulong_long_value);
                  break;
              case FLOAT32_TYPE: /* 4 bytes */
                  float_value = deserialize_float32(field_value_buffer);
                  dynmessage_put_float32_field_value(dyn_message, (char *)field_name, &float_value);
                  break;
              case FLOAT64_TYPE: /* 8 bytes */
                  double_value = deserialize_float64(field_value_buffer);
                  dynmessage_put_float64_field_value(dyn_message, (char *)field_name, &double_value);
                  break;
              case STRING_TYPE: /* n bytes */
                  {
                      unsigned char string_value[len + 1]; /* +1 for \0 */
                      strslice(string_value, field_value_buffer, 0, len);
                      string_value[len] = '\0';
                      dynmessage_put_string_field_value(dyn_message, (char *)field_name, (char *)string_value);
                  }
                  break;
              case NO_TYPE: /* 0 bytes */
                  break;
              }
              start_idx = start_idx + len;
              SAFE_FREE(field_name); /* done with this variable */
              SAFE_FREE(field_value_buffer); /* done with this variable */
          }
        } else {
          log_error_format(
            "dynamicmessage_deserialize_bin: empty message %s\n", message_name);
        }
    }
    return (void *)dyn_message;
}

/**
 * Function to serialize a dynamic message object into a sequence of bytes
 * (binary version).
 *
 * @param object the dynamic message to serialize.
 * @param serdi serialized_data_info structure to store the
 *              serialized dynamic message object.
 */
extern void
dynmessage_serialize_bin(void *object, serialized_data_info *serdi) {
    int i, len;
    unsigned char * data;
    dynamicmessage *message = (dynamicmessage *)object;
    dyn_field_list * field_list;
    int message_length = calc_dynmessage_serialized_len(message);

    if (message_length > DYN_MSG_MIN_LEN) {
      serdi->ser_data_len = message_length;
      serdi->ser_data = (unsigned char *)SAFE_MALLOC(message_length * sizeof(unsigned char));
      /* use a local variable instead of modifying the actual one */
      data = serdi->ser_data;
      /* 'Dynamic Message Start' (4 bytes) */
      serialize_int32(data, DYN_MSG_START);
      data += BYTES_4;
      /* dynamic message length (total) (4 bytes) */
      serialize_int32(data, message_length);
      data += BYTES_4;
      /* dynamic message name length (4 bytes) */
      len = strlen(message->name);
      serialize_int32(data, len);
      data += BYTES_4;
      /* dynamic message name (m bytes) */
      memcpy(data, message->name, len);
      data += len;
      /* dynamic message number of fields (n) (4 bytes) */
      serialize_int32(data, message->field_count);
      data += BYTES_4;
      /* get a list of all dynamic message fields */
      field_list = dynmessage_get_fields(message);
      /* serialize all dynamic fields */
      for (i=0; i<field_list->list_length;i++) {
        dyn_field *field = field_list->list[i]; /* save field info */
        /* determine field size */
        int value_size = DYN_FIELD_TYPE_SER_SIZE[field->type];
        if (field->type == STRING_TYPE) {
            value_size = strlen(field->value->string_value);
        }
        /* field length (total) (4 bytes) */
        len = DYN_FIELD_FIXED_LEN + strlen(field->name) + value_size;
        serialize_int32(data, len);
        data += BYTES_4;
        /* field name length (4 bytes) */
        len = strlen(field->name);
        serialize_int32(data, len);
        data += BYTES_4;
        /* field name (k bytes) */
        memcpy(data, field->name, len);
        data += len;
        /* field type (4 bytes) */
        serialize_int32(data, field->type);
        data += BYTES_4;
        /* field value length (4 bytes) */
        serialize_int32(data, value_size);
        data += BYTES_4;
        /* field value (l bytes) */
        switch(field->type) {
        case ENUMERATION_TYPE: /* 4 bytes */
            serialize_int32(data, (field->value->enum_value));
            break;
        case INT16_TYPE: /* 2 bytes */
            serialize_int16(data, (field->value->int16_value));
            break;
        case UNSIGNED_INT16_TYPE: /* 2 bytes */
            serialize_int16(data, (field->value->uint16_value));
            break;
        case INT32_TYPE: /* 4 bytes */
            serialize_int32(data, (field->value->int32_value));
            break;
        case UNSIGNED_INT32_TYPE: /* 4 bytes */
            serialize_int32(data, (unsigned long int)(field->value->uint32_value));
            break;
        case INT64_TYPE: /* 8 bytes */
            serialize_int64(data, (long long int)(field->value->int64_value));
            break;
        case UNSIGNED_INT64_TYPE: /* 8 bytes */
            serialize_int64(data, (unsigned long long int)(field->value->uint64_value));
            break;
        case FLOAT32_TYPE: /* 4 bytes */
            serialize_float32(data, field->value->float32_value);
            break;
        case FLOAT64_TYPE: /* 8 bytes */
            serialize_float64(data, field->value->float64_value);
            break;
        case STRING_TYPE: /* n bytes */
            memcpy(data, field->value->string_value, value_size);
            break;
        case NO_TYPE: /* 0 bytes */
            break;
        }
        data += value_size;
        free(field); /* done with this field */
      }
      free(field_list->list); /* done with the field list */

      free(field_list); /* done with the whole list */
    }
}
