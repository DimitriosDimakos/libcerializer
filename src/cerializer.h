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
 * Module providing serialization operations on primitive types.
 */

#ifndef CERIALIZER_H_
#define CERIALIZER_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/* Structure to hold the serialized data information. */
typedef struct _serialized_data_info_struct {
    unsigned char *ser_data; /* actual serialized content */
    int ser_data_len; /* content length */
} serialized_data_info;

/**
 * Function to de-serialize the data of sequence of bytes.
 *
 * @param data the sequence of bytes representing the data.
 * @param data_len length in bytes of the byte sequence.
 *
 * @return reference to the de-serialized data object.
 */
typedef void *(*deserialize_data)(unsigned char *data, int data_len);

/**
 * Function to serialize an object(content) into a sequence of bytes.
 *
 * @param object the object to serialize.
 * @param serdi serialized_data_info structure to store the serialized object.
 */
typedef void (*serialize_object)(void *object, serialized_data_info *serdi);

/* Structure to hold the data serializer. */
typedef struct _data_serializer_struct {
    deserialize_data deserialize_func; /* function to de-serialize */
    serialize_object serialize_func; /* function to serialize */
    char *data_type_name; /* specific type of content message */
} data_serializer;

/**
 * Copies a slice of the buffer pointed to by src, to the buffer pointed to by dest.
 *
 * @param dest destination buffer to store the src sliced buffer.
 * @param src origin buffer to retrieve the sliced data.
 *
 * @return reference to the destination buffer.
 */
extern unsigned char *
strslice(unsigned char *dest, const unsigned char *src, size_t start_idx, size_t n);

/**
 * Utility function to clear contents of a serialized_data_info structure;
 *
 * @param serdi serialized_data_info structure reference (not NULL).
 */
extern void
clear_serialized_data_info(serialized_data_info * serdi);

/**
 * Serialize a 16-bit integer into a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer to store the serialized 16-bit integer.
 * @param i 16-bit integer value.
 */
extern void
serialize_int16(unsigned char *buf, unsigned int i);

/**
 * De-serialize a 16-bit integer from a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer containing the serialized 16-bit integer.
 *
 * @return de-serialized 16-bit integer value.
 */
extern int
deserialize_int16(unsigned char *buf);

/**
 * De-serialize a 16-bit unsigned from a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer containing the serialized 16-bit unsigned integer.
 *
 * @return de-serialized 16-bit unsigned integer value.
 */
extern unsigned int
deserialize_uint16(unsigned char *buf);

/**
 * Serialize a 32-bit integer into a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer to store the serialized 32-bit integer.
 * @param i 32-bit integer value.
 */
extern void
serialize_int32(unsigned char *buf, unsigned long int i);

/**
 * De-serialize a 32-bit integer from a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer containing the serialized 32-bit integer.
 *
 * @return de-serialized 32-bit integer value.
 */
extern long int
deserialize_int32(unsigned char *buf);

/**
 * De-serialize a 32-bit unsigned integer from a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer containing the serialized 32-bit unsigned integer.
 *
 * @return de-serialized 32-bit unsigned integer value.
 */
extern unsigned long int
deserialize_uint32(unsigned char *buf);

/**
 * Serialize a 64-bit integer into a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer to store the serialized 64-bit integer.
 * @param i 64-bit integer value.
 */
extern void
serialize_int64(unsigned char *buf, unsigned long long int i);

/**
 * De-serialize a 64-bit integer from a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer containing the serialized 64-bit unsigned integer.
 *
 * @return de-serialized 64-bit integer value.
 */
extern long long int
deserialize_int64(unsigned char *buf);

/**
 * De-serialize a 64-bit unsigned integer from a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer containing the serialized 64-bit unsigned integer.
 *
 * @return de-serialized 64-bit unsigned integer value.
 */
extern unsigned long long int
deserialize_uint64(unsigned char *buf);

/**
 * Serialize a 32-bit floating point number(IEE 754 version).
 *
 * @param buf sequence of bytes buffer to store the
 *            serialized 32-bit floating point number.
 * @param f 32-bit floating point number value.
 */
extern void
serialize_float32(unsigned char *buf, float f);

/**
 * De-serialize a 32-bit floating point number from a sequence of bytes buffer(IEE 754 version).
 *
 * @param buf sequence of bytes buffer containing the serialized 32-bit floating point number.
 *
 * @return de-serialized 32-bit floating point number value.
 */
extern long double
deserialize_float32(unsigned char *buf);

/**
 * Serialize a 64-bit floating point number(IEE 754 version).
 *
 * @param buf sequence of bytes buffer to store the
 *            serialized 64-bit floating point number.
 * @param f 64-bit floating point number value.
 */
extern void
serialize_float64(unsigned char *buf, double f);

/**
 * De-serialize a 64-bit floating point number from a sequence of bytes buffer(IEE 754 version).
 *
 * @param buf sequence of bytes buffer containing the serialized 64-bit floating point number.
 *
 * @return de-serialized 64-bit floating point number value.
 */
extern double
deserialize_float64(unsigned char *buf);

#ifdef  __cplusplus
}
#endif

#endif /* CSERIALIZER_H_ */
