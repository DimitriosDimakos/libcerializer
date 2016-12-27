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
 * Simple test to demonstrate the functionality of libcerializer.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>

#include <dynmessage_cerializer.h>

typedef struct _heartbeat_message_struct {
    int message_source;
    int message_destination;
    int message_id;
    char *message_name;
    int message_counter;
    long int time_stamp;
    long int time_stamp_us;
    float message_version;
    double system_version;
} heartbeat_message;

int main(int argc, char **argv) {
    int i;
    struct timeval tv;
    heartbeat_message message_out, message_in;
    dynamicmessage dynmessage_out;
    dynamicmessage *dynmessage_in = NULL;
    dyn_field value;
    dyn_field_list * field_list = NULL;
    serialized_data_info serdi;

    /**
     * Set values to our c structure like 'Heartbeat' message.
     */
    message_out.message_source = 1;
    message_out.message_destination = 0;
    message_out.message_id = 6;
    message_out.message_name = "Heartbeat";
    message_out.message_counter = 1;
    gettimeofday(&tv, NULL);
    message_out.time_stamp = tv.tv_sec;
    message_out.time_stamp_us = tv.tv_usec;
    message_out.message_version = 1.25f;
    message_out.system_version = 2.375;

    /*
     * create a dynamic message out of the 'Heartbeat' c structure.
     */
    dynmessage_init(&dynmessage_out, "Heartbeat");
    /* add message fields and corresponding values */
    dynmessage_put_field_and_value(&dynmessage_out, "message_source", INT32_TYPE, &message_out.message_source);
    dynmessage_put_field_and_value(&dynmessage_out, "message_destination", INT32_TYPE, &message_out.message_destination);
    dynmessage_put_field_and_value(&dynmessage_out, "message_id", INT32_TYPE, &message_out.message_id);
    dynmessage_put_field_and_value(&dynmessage_out, "message_name", STRING_TYPE, message_out.message_name);
    dynmessage_put_field_and_value(&dynmessage_out, "message_counter", INT32_TYPE, &message_out.message_counter);
    dynmessage_put_field_and_value(&dynmessage_out, "time_stamp", UNSIGNED_INT32_TYPE, &message_out.time_stamp);
    dynmessage_put_field_and_value(&dynmessage_out, "time_stamp_us", UNSIGNED_INT32_TYPE, &message_out.time_stamp_us);
    dynmessage_put_field_and_value(&dynmessage_out, "message_version", FLOAT32_TYPE, &message_out.message_version);
    dynmessage_put_field_and_value(&dynmessage_out, "system_version", FLOAT64_TYPE, &message_out.system_version);
    /*
     * Print output 'Heartbeat' dynamic message layout.
     */
    fprintf(stdout, "dynmessage_out.field_count = %d\n", dynmessage_out.field_count);
    field_list = dynmessage_get_fields(&dynmessage_out);
    if (field_list->list_length > 0) {
        for (i=0; i<field_list->list_length;i++) {
            dyn_field *field = field_list->list[i];
            fprintf(stdout, "dynmessage_out.field->name %s\n", field->name);
            free(field);
        }
        free(field_list->list); /* free internal resources */
    }
    free(field_list); /* done with this variable instance */

    /**
     * Serialize the 'Heartbeat' dynamic message
     */
    dynmessage_serialize_bin((void *)&dynmessage_out, &serdi);

    /**
     * De-serialize data into a dynamic message.
     */
    dynmessage_in = (dynamicmessage *)dynmessage_deserialize_bin(serdi.ser_data, serdi.ser_data_len);
    /*
     * Print input 'Heartbeat' dynamic message layout.
     */
    fprintf(stdout, "dynmessage_in->field_count = %d\n", dynmessage_in->field_count);
    field_list = dynmessage_get_fields(dynmessage_in);

    if (field_list->list_length > 0) {
        for (i=0; i<field_list->list_length;i++) {
            dyn_field *field = field_list->list[i];
            fprintf(stdout, "dynmessage_in->field->name %s\n", field->name);
            free(field);
        }
        free(field_list->list);/* free internal resources */
    }
    free(field_list);/* done with this variable instance */

    /**
     * Set values to our c structure like 'Heartbeat' message
     * from our de-serialized dynamic message.
     */
    dynmessage_get_field(dynmessage_in, "message_source", &value);
    message_in.message_source = value.value->int32_value;
    dynmessage_get_field(dynmessage_in, "message_destination", &value);
    message_in.message_destination = value.value->int32_value;
    dynmessage_get_field(dynmessage_in, "message_id", &value);
    message_in.message_id = value.value->int32_value;
    dynmessage_get_field(dynmessage_in, "message_name", &value);
    message_in.message_name = strdup(value.value->string_value);
    dynmessage_get_field(dynmessage_in, "message_counter", &value);
    message_in.message_counter = value.value->int32_value;
    dynmessage_get_field(dynmessage_in, "time_stamp", &value);
    message_in.time_stamp = value.value->uint32_value;
    dynmessage_get_field(dynmessage_in, "time_stamp_us", &value);
    message_in.time_stamp_us = value.value->uint32_value;
    dynmessage_get_field(dynmessage_in, "message_version", &value);
    message_in.message_version = value.value->float32_value;
    dynmessage_get_field(dynmessage_in, "system_version", &value);
    message_in.system_version = value.value->float64_value;

    /*
     * Print values
     */
    fprintf(stdout, "message_in.message_source  = %d\n", message_in.message_source);
    fprintf(stdout, "message_out.message_source = %d\n", message_out.message_source);
    fprintf(stdout, "message_in.message_destination  = %d\n", message_in.message_destination);
    fprintf(stdout, "message_out.message_destination = %d\n", message_out.message_destination);
    fprintf(stdout, "message_in.message_id  = %d\n", message_in.message_id);
    fprintf(stdout, "message_out.message_id = %d\n", message_out.message_id);
    fprintf(stdout, "message_in.message_name  = %s\n", message_in.message_name);
    fprintf(stdout, "message_out.message_name = %s\n", message_out.message_name);
    fprintf(stdout, "message_in.message_counter  = %d\n", message_in.message_counter);
    fprintf(stdout, "message_out.message_counter = %d\n", message_out.message_counter);
    fprintf(stdout, "message_in.time_stamp  = %li\n", message_in.time_stamp);
    fprintf(stdout, "message_out.time_stamp = %li\n", message_out.time_stamp);
    fprintf(stdout, "message_in.time_stamp_us  = %li\n", message_in.time_stamp_us);
    fprintf(stdout, "message_out.time_stamp_us = %li\n", message_out.time_stamp_us);
    fprintf(stdout, "message_in.message_version  = %f\n", message_in.message_version);
    fprintf(stdout, "message_out.message_version = %f\n", message_out.message_version);
    fprintf(stdout, "message_in.system_version  = %f\n", message_in.system_version);
    fprintf(stdout, "message_out.system_version = %f\n", message_out.system_version);
    /*
     * Free resources.
     */
    clear_serialized_data_info(&serdi);
    dynmessage_free(&dynmessage_out);
    dynmessage_destroy(dynmessage_in);

    exit(0);
}
