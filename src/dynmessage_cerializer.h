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

#ifndef DYNMESSAGE_CERIALIZER_H_
#define DYNMESSAGE_CERIALIZER_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <cerializer.h>
#include <dynmessage.h>

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
dynmessage_deserialize_bin(unsigned char *data, int data_len);

/**
 * Function to serialize a dynamic message object into a sequence of bytes
 * (binary version).
 *
 * @param object the dynamic message to serialize.
 * @param serdi serialized_data_info structure to store the
 *              serialized dynamic message object.
 */
extern void
dynmessage_serialize_bin(void *object, serialized_data_info *serdi);

#ifdef  __cplusplus
}
#endif

#endif /* DYNMESSAGE_CERIALIZER_H_ */
