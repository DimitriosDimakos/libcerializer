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
 * Utility program which generates c source code
 * assorted for the libcerializer library module. The
 * program is provided with a file containing the xml
 * description of a structure desired to be (de)serialized.
 * The program output is a set of c source code files,
 * containing the c structures as well as convenience
 * functions related to the libcerializer library module.
 *
 * That program uses(source code) of a third party XML parsing
 * C library: ezXML 0.8.6
 *
 * Example of a cerializer dynamic message definition
 * file(cerialized.dmd):
 * -----------------------------------------------------
 * <crealized_dmd>
 *     <message name="my message">
 *         <field name="field 1">INT32_TYPE</field>
 *         <field name="field 2">FLOAT32_TYPE</field>
 *         <field name="field 3">STRING_TYPE</field>
 *     </message>
 * </crealized_dmd>
 *
 * Allowed field types:
 * -----------------------------------------------------
 * ENUMERATION_TYPE
 * INT16_TYPE
 * UNSIGNED_INT16_TYPE
 * INT32_TYPE
 * UNSIGNED_INT32_TYPE
 * INT64_TYPE
 * UNSIGNED_INT64_TYPE
 * FLOAT32_TYPE
 * FLOAT64_TYPE
 * STRING_TYPE
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ezxml.h"

#define H_SET_FNAME_POST_FIX "_set_h"
#define H_SET_FNAME_POST_FIX_LEN 6

#define CV_H_SET_FNAME_POST_FIX  "_set_c_h"
#define CV_H_SET_FNAME_POST_FIX_LEN 8

#define CV_C_SET_FNAME_POST_FIX  "_set_c_c"
#define CV_C_SET_FNAME_POST_FIX_LEN 8

#ifdef TEST
/**
 * Function to test the implementation of cerializertool.
 */
static void
test_gen_source_set(void);
#endif

/* structure to store message information */
typedef struct _message_info_struct_ {
    char * message_name; /* name of message */
    char ** field_names; /* name of all message fields */
    char ** field_types; /* types of all message fields */
    int field_count; /* count of message fields */
} message_info_struct;

/* allowed field value types (enumerated representation) */
typedef enum _allowed_value_types {
    ENUMERATION_TYPE,
    INT16_TYPE,
    UNSIGNED_INT16_TYPE,
    INT32_TYPE,
    UNSIGNED_INT32_TYPE,
    INT64_TYPE,
    UNSIGNED_INT64_TYPE,
    FLOAT32_TYPE,
    FLOAT64_TYPE,
    STRING_TYPE,
} allowed_value_types;

/* allowed field value types (string representation) */
static char * allowed_value_types_text[] = {
    "ENUMERATION_TYPE",
    "INT16_TYPE",
    "UNSIGNED_INT16_TYPE",
    "INT32_TYPE",
    "UNSIGNED_INT32_TYPE",
    "INT64_TYPE",
    "UNSIGNED_INT64_TYPE",
    "FLOAT32_TYPE",
    "FLOAT64_TYPE",
    "STRING_TYPE"
};

/* allowed field value types (c type representation) */
static char * allowed_value_types_ctype[] = {
    "unsigned int",
    "int",
    "int",
    "int",
    "long",
    "long long",
    "unsigned long long",
    "float",
    "double",
    "char *"
};

/* allowed field value types (c union value representation) */
static char * allowed_value_types_cunion[] = {
    "enum_value",
    "int16_value",
    "uint16_value",
    "int32_value",
    "uint32_value",
    "int64_value",
    "uint64_value",
    "float32_value",
    "float64_value",
    "string_value"
};

/**
 * Test whether the provided character is an alphabet letter.
 *
 * @param c character value.
 *
 * @return Non zero if the provided character is
 *         an alphabet letter, zero otherwise.
 */
static int
is_alpha(char c) {
    int result = 0;
    int i = (int)c;
    if ((i>= 65 && i <= 90) || (i>= 97 && i <= 122)) {
        result ++;
    }
    return result;
}

/**
 * Test whether the provided character is a number.
 *
 * @param c character value.
 *
 * @return Non zero if the provided character is a number, zero otherwise.
 */
static int
is_num(char c) {
    int result = 0;
    int i = (int)c;
    if (i>= 48 && i <= 57) {
        result ++;
    }
    return result;
}

/**
 * Tests whether a provided reference is null an exits program execution if so,
 * printing on screen  a related error message.
 *
 * @param ptr the provided reference to be tested.
 * @param message error message.
 */
static void
exit_if_null(void * ptr, char * message) {
    if (ptr == NULL) {
        fprintf(stderr, "[ERROR]: %s\n", message);
        exit(1);
    }
}

/**
 * Tests whether the provided field value type(represented as string) is supported.
 *
 * @param field_value_type field value type(represented as string) under test.
 * @return Non zero if the provided field value type is supported, zero otherwise.
 */
static int
valid_field_value_type(char * field_value_type) {
    int result = 0;
    int i;
    for (i=0;i<10;i++) {
        if (strcmp(allowed_value_types_text[i], field_value_type) == 0) {
            result++;
            break;
        }
    }
    return result;
}

/**
 * function to get the c primitive type in textual representation of the provided field value type.
 *
 * @param field_value_type field value type(represented as string).
 * @return c primitive type in textual representation of the provided field value type.
 */
static char *
get_field_value_type_text(char * field_value_type) {
    int i;
    for (i=0;i<10;i++) {
        if (strcmp(allowed_value_types_text[i], field_value_type) == 0) {
            return(allowed_value_types_ctype[i]);
        }
    }
    return "UNSUPPORTED";
}

/**
 * function to get the c primitive type union value in textual representation
 * of the provided field value type.
 *
 * @param field_value_type field value type(represented as string).
 * @return c primitive type union value in textual representation
 *         of the provided field value type.
 */
static char *
get_field_value_type_union_text(char * field_value_type) {
    int i;
    for (i=0;i<10;i++) {
        if (strcmp(allowed_value_types_text[i], field_value_type) == 0) {
            return(allowed_value_types_cunion[i]);
        }
    }
    return "UNSUPPORTED";
}

/**
 * Function to get a proper name for a c language element, taking into
 * account the name specified from the user. In case there are no
 * issues with the user provided name, it shall be used as the proper
 * name. If there are issues, this function shall try to resolve
 * them.
 *
 * @param user_prov_name name of the name provided by the user
 *
 * @return the proper name for the c element.
 */
static char *
get_c_proper_name(char * user_prov_name) {
    int i, j;
    char * proper_name;

    exit_if_null(user_prov_name, "NULL user provided name!");
    proper_name = (char *) malloc(strlen(user_prov_name) + 1);
    exit_if_null(proper_name, "unable to allocate enough memory!");
    j = 0;proper_name[j] = '\0';

    for (i=0; i<strlen(user_prov_name); i++) {
        if (j == 0) { /* first letter should be an alphabet letter */
            if (is_alpha(user_prov_name[i])) {
                proper_name[j++] = user_prov_name[i];
            }
        } else if (i == strlen(user_prov_name) -1) {
            if (is_alpha(user_prov_name[i]) || is_num(user_prov_name[i])) {
                proper_name[j++] = user_prov_name[i];
            }
        } else {
            if (is_alpha(user_prov_name[i]) || is_num(user_prov_name[i])
                || user_prov_name[i] == '_' || user_prov_name[i] == ' ') {
                if (user_prov_name[i] == ' ') {
                    proper_name[j++] = '_';
                } else {
                    proper_name[j++] = user_prov_name[i];
                }
            }
        }
    }
    proper_name[j] = '\0';
    return proper_name;
}

/**
 * Function to open standard generated files that will contain the source code for the
 * given message name;
 *
 * @param h_fptr header file containing the c structure of the message.
 * @param cv_h_fptr header file that will contain the declaration of convenience
 *        (de)serialization functions for the message.
 * @param cv_c_fptrm header file that will contain the implementation of convenience
 *        (de)serialization functions for the message.
 * @param message_name name of the message.
 */
static void
open_standard_gen_files(FILE **h_fptr, FILE **cv_h_fptr, FILE **cv_c_fptr, char * message_name) {
    /* generated file names */
    int len = strlen(message_name);
    char h_fname[len + H_SET_FNAME_POST_FIX_LEN + 1];
    char cv_h_fname[len + CV_H_SET_FNAME_POST_FIX_LEN + 1];
    char cv_c_fname[len + CV_C_SET_FNAME_POST_FIX_LEN + 1];

    sprintf(h_fname, "%s%s", message_name, H_SET_FNAME_POST_FIX);
    sprintf(cv_h_fname, "%s%s", message_name, CV_H_SET_FNAME_POST_FIX);
    sprintf(cv_c_fname, "%s%s", message_name, CV_C_SET_FNAME_POST_FIX);

    if ((*h_fptr = fopen(h_fname, "w+")) == NULL) {
        fprintf(stderr,"[ERROR]: cannot create %s! aborting\n", h_fname);
        exit(1);
    }
    if ((*cv_h_fptr = fopen(cv_h_fname, "w+")) == NULL) {
        fprintf(stderr,"[ERROR]: cannot create %s! aborting\n", cv_h_fname);
        exit(1);
    }
    if ((*cv_c_fptr = fopen(cv_c_fname, "w+")) == NULL) {
        fprintf(stderr,"[ERROR]: cannot create %s! aborting\n", cv_c_fname);
        exit(1);
    }
}

/**
 * Function to prepare standard generated files that will contain the source code for the
 * given message name;
 *
 * @param h_fptr header file containing the c structure of the message.
 * @param cv_h_fptr header file that will contain the declaration of convenience
 *        (de)serialization functions for the message.
 * @param cv_c_fptrm header file that will contain the implementation of convenience
 *        (de)serialization functions for the message.
 * @param message_name name of the message.
 */
static void
prepare_standard_gen_files(FILE *h_fptr, FILE *cv_h_fptr, FILE *cv_c_fptr, char * message_name) {
    time_t timeval = time(NULL);
    /* common code for message c structure definition */
    fprintf(h_fptr, "/**\n * Definition of %s message.\n * Generated by crealizertool at %s */\n\n",
        message_name, ctime(&timeval));

    fprintf(h_fptr,
        "#ifndef _%s_set_h_\n#define _%s_set_h_\n\n#ifdef  __cplusplus\nextern \"C\" {\n#endif\n",
        message_name, message_name);

    /* header for convenience functions */
    fprintf(cv_h_fptr,
        "\n/**\n * Convenience functions to send/receive a serialized %s message.\n"
        " * Generated by crealizertool at %s */\n\n",
        message_name,  ctime(&timeval));
    fprintf(cv_h_fptr,
        "#ifndef _%s_set_c_h_\n#define _%s_set_c_h_\n\n#ifdef  __cplusplus\nextern \"C\" {\n#endif\n",
        message_name, message_name);
    fprintf(cv_h_fptr, "\n#include \"%s_set.h\"\n", message_name);
    fprintf(cv_h_fptr, "#include \"cerializer.h\"\n");
    fprintf(cv_h_fptr, "#include \"dynmessage.h\"\n");

    fprintf(cv_h_fptr,
        "\n/**\n"
        " * Convenience function to serialize a %s message object\n"
        " * into a sequence of bytes(as a dynamicmessage).\n"
        " *\n"
        " * @param object reference to the %s message to serialize(not NULL).\n"
        " * @param serdi reference to the serialized_data_info structure(not NULL)\n"
        " *              to store the serialized %s message object representation.\n"
        " *\n"
        " * @return Non-zero upon successful serialization, zero otherwise.\n"
        " */\n"
        "extern int\n"
        "c_serialize_%s(%s *object, serialized_data_info *serdi);\n",
        message_name, message_name, message_name, message_name, message_name);

    fprintf(cv_h_fptr,
        "\n/**\n"
        " * Convenience function to deserialize a sequence of bytes representing\n"
        " * a %s message (as a dynamicmessage).\n"
        " *\n"
        " * @param serdi reference to the serialized_data_info structure(not NULL)\n"
        " *        containing the serialized %s message object.\n"
        " * @param object reference to the deserialized %s message(not NULL).\n"
  		" *\n"
        " * @return Non-zero upon successful de-serialization, zero otherwise.\n"
        " */\n"
        "extern int\n"
        "c_deserialize_%s(serialized_data_info *serdi, %s *object);\n",
        message_name, message_name, message_name, message_name, message_name);

    fprintf(cv_h_fptr,
        "\n/**\n"
        " * Convenience function to convert a %s message object\n"
        " * into a dynamic message object.\n"
        " *\n"
        " * @param object reference to the %s message to convert(not NULL).\n"
        " * @param dm reference to the dynamicmessage structure(not NULL),\n"
        " *              to store the representation of a %s message object.\n"
        " *\n"
        " * @return Non-zero upon successful conversion, zero otherwise.\n"
        " */\n"
        "extern int\n"
        "c_conv_%s_2dm(%s *object, dynamicmessage *dm);\n",
        message_name, message_name, message_name, message_name, message_name);

    fprintf(cv_h_fptr,
        "\n/**\n"
        " * Convenience function to convert dynamic message into\n"
        " * a %s message object.\n"
        " *\n"
        " * @param dm reference to the dynamicmessage structure(not NULL),\n"
        " *           containing the representation of a %s message object.\n"
        " * @param object reference to the converted %s message(not NULL).\n"
        " *\n"
        " * @return Non-zero upon successful conversion, zero otherwise.\n"
        " */\n"
        "extern int\n"
        "c_conv_dm_2%s(dynamicmessage *dm, %s *object);\n",
        message_name, message_name, message_name, message_name, message_name);

    /* implementation for convenience functions */
    fprintf(cv_c_fptr,
        "\n/**\n * Convenience functions to send/receive a serialized %s message.\n"
        " * Generated by crealizertool at %s */\n\n",
        message_name,  ctime(&timeval));
    fprintf(cv_c_fptr, "#include \"%s_set_c.h\"\n", message_name);
    fprintf(cv_c_fptr, "#include \"cerializer.h\"\n");
    fprintf(cv_c_fptr, "#include \"dynmessage.h\"\n");
    fprintf(cv_c_fptr, "#include \"dynmessage_cerializer.h\"\n");
}

/**
 * Function to generate the implementation source code for the given message name;
 *
 * @param h_fptr header file containing the c structure of the message.
 * @param cv_c_fptrm header file that will contain the implementation of convenience
 *        (de)serialization functions for the message.
 * @param message_info reference to message information structure
 */
static void
generate_implementation(FILE *h_fptr, FILE *cv_c_fptr, message_info_struct *message_info) {
    int i;
    /* definition of the c structure for the message */
    fprintf(h_fptr, "\n/* structure to store %s message information */\n"
        "typedef struct _%s_struct_ {\n", message_info->message_name, message_info->message_name);
    for (i=0; i<message_info->field_count; i++) {
        fprintf(h_fptr, "    %s %s;\n",
            get_field_value_type_text(message_info->field_types[i]), message_info->field_names[i]);
    }
    fprintf(h_fptr, "} %s;\n", message_info->message_name);

    /* implementation of the convenience functions */
    fprintf(cv_c_fptr,
        "\n/**\n"
        " * Convenience function to serialize a %s message object\n"
        " * into a sequence of bytes(as a dynamicmessage).\n"
        " *\n"
        " * @param object reference to the %s message to serialize(not NULL).\n"
        " * @param serdi reference to the serialized_data_info structure(not NULL)\n"
        " *              to store the serialized %s message object representation.\n"
        " *\n"
        " * @return Non-zero upon successful serialization, zero otherwise.\n"
        " */\n"
        "extern int\n"
        "c_serialize_%s(%s *object, serialized_data_info *serdi) {\n"
        "    int result = 0;\n",
        message_info->message_name, message_info->message_name,
        message_info->message_name, message_info->message_name,
        message_info->message_name);
    fprintf(cv_c_fptr, "    if (object != NULL && serdi != NULL) {\n"
                       "        dynamicmessage dm;\n"
                       "        /* convert %s object to a dynamicmessage object */\n"
                       "        if (c_conv_%s_2dm(object, &dm)) {\n"
                       "            /* Serialize the '%s' dynamicmessage object */\n"
                       "            dynmessage_serialize_bin((void *)&dm, serdi);\n"
                       "            dynmessage_free(&dm);\n"
                       "            if (serdi->ser_data != NULL) {\n"
                       "                result++;\n"
                       "            }\n"
                       "        }\n"
                       "    }\n",
        message_info->message_name, message_info->message_name,
        message_info->message_name);
    fprintf(cv_c_fptr, "    return (result);\n}\n");

    fprintf(cv_c_fptr,
        "\n/**\n"
        " * Convenience function to deserialize sequence of bytes representing\n"
        " * a %s message (as dynamicmessage).\n"
        " *\n"
        " * @param serdi reference to the serialized_data_info structure(not NULL)\n"
        " *        containing the serialized %s message object.\n"
        " * @param object reference to the deserialized %s message(not NULL).\n"
  		" *\n"
        " * @return Non-zero upon successful de-serialization, zero otherwise.\n"
        " */\n"
        "extern int\n"
        "c_deserialize_%s(serialized_data_info *serdi, %s *object) {\n"
        "    int result = 0;\n",
        message_info->message_name,
        message_info->message_name,
        message_info->message_name,
        message_info->message_name,
        message_info->message_name);
    fprintf(cv_c_fptr, "    if (object != NULL && edi != NULL) {\n"
                       "        /* decode data into a dynamicmessage object */\n"
                       "        dynamicmessage *dm = dynmessage_deserialize_bin(serdi->ser_data, serdi->ser_data_len);\n"
                       "        if (sm) {\n"
                       "            /* convert dynamicmessage object to a '%s' object */\n"
                       "            if (c_conv_dm_2%s(dm, object)) {\n"
                       "                result++;\n"
                       "            }\n"
                       "            dynamicmessage_destroy(sm);\n"
                       "        }\n"
                       "    }\n",
        message_info->message_name, message_info->message_name);
    fprintf(cv_c_fptr, "    return (result);\n}\n");

    fprintf(cv_c_fptr,
        "\n/**\n"
        " * Convenience function to convert a %s message object\n"
        " * into a dynamic message object.\n"
        " *\n"
        " * @param object reference to the %s message to convert(not NULL).\n"
        " * @param dm reference to the dynamicmessage structure(not NULL),\n"
        " *              to store the representation of a %s message object.\n"
        " *\n"
        " * @return Non-zero upon successful conversion, zero otherwise.\n"
        " */\n"
        "extern int\n"
        "c_conv_%s_2dm(%s *object, dynamicmessage *dm) {\n"
        "    int result = 0;\n",
        message_info->message_name, message_info->message_name,
        message_info->message_name, message_info->message_name,
        message_info->message_name);

    if (message_info->field_count > 0) {
        fprintf(cv_c_fptr, "    if (object != NULL && dm != NULL) {\n");
        fprintf(cv_c_fptr, "        int error = 0;\n");
        fprintf(cv_c_fptr, "        dynmessage_init(dm, \"%s\");\n", message_info->message_name);
        for (i=0; i<message_info->field_count; i++) {
            if (strcmp(message_info->field_types[i], "STRING_TYPE") == 0) {
                fprintf(cv_c_fptr,
                    "        if (object->%s == NULL) {\n            error++;\n        } else {\n",
                message_info->field_names[i]);

                fprintf(cv_c_fptr,
                    "            dynmessage_put_field_and_value(dm, \"%s\", %s, object->%s);\n",
                    message_info->field_names[i], message_info->field_types[i], message_info->field_names[i]);
                fprintf(cv_c_fptr, "        }\n");
            } else {
                fprintf(cv_c_fptr,
                    "        dynmessage_put_field_and_value(dm, \"%s\", %s, &object->%s);\n",
                    message_info->field_names[i], message_info->field_types[i], message_info->field_names[i]);
            }
        }
        fprintf(cv_c_fptr,
            "        if (!error) {\n            result++;\n        }\n    }\n");
    }
    fprintf(cv_c_fptr, "    return (result);\n}\n");

    fprintf(cv_c_fptr,
        "\n/**\n"
        " * Convenience function to convert dynamic message into\n"
        " * a %s message object.\n"
        " *\n"
        " * @param dm reference to the dynamicmessage structure(not NULL),\n"
        " *           containing the representation of a %s message object.\n"
        " * @param object reference to the converted %s message(not NULL).\n"
        " *\n"
        " * @return Non-zero upon successful conversion, zero otherwise.\n"
        " */\n"
        "extern int\n"
        "c_conv_dm_2%s(dynamicmessage *dm, %s *object) {\n"
        "    int result = 0;\n",
        message_info->message_name, message_info->message_name,
        message_info->message_name, message_info->message_name,
        message_info->message_name);
    if (message_info->field_count > 0) {
        fprintf(cv_c_fptr,
            "    if (object != NULL && dm != NULL) {\n"
            "        dyn_field field;\n"
            "        int error = 0;\n");
        for (i=0; i<message_info->field_count; i++) {
            fprintf(cv_c_fptr, "        dynmessage_get_field(dm, \"%s\", &field);\n",
                message_info->field_names[i]);
            fprintf(cv_c_fptr, "        if (field.seq == -1) {\n"
                               "            error++;\n"
                               "        } else {\n");
            fprintf(cv_c_fptr, "            object->%s = field.value->%s;\n",
                message_info->field_names[i],
                get_field_value_type_union_text(message_info->field_types[i]));
            fprintf(cv_c_fptr, "        }\n");
        }
        fprintf(cv_c_fptr,
            "        if (!error) {\n            result++;\n        }\n    }\n");
    }
    fprintf(cv_c_fptr, "    return (result);\n}\n");
}

/**
 * Function to finalize standard generated files that will contain the source code for the
 * given message name;
 *
 * @param h_fptr header file containing the c structure of the message.
 * @param cv_h_fptr header file that will contain the declaration of convenience
 *        (de)serialization functions for the message.
 * @param cv_c_fptrm header file that will contain the implementation of convenience
 *        (de)serialization functions for the message.
 * @param message_name name of the message.
 */
static void
finalize_standard_gen_files(FILE *h_fptr, FILE *cv_h_fptr, FILE *cv_c_fptr, char * message_name) {
    fprintf(h_fptr,
        "\n#ifdef  __cplusplus\n}\n#endif\n\n#endif /* _%s_set_h_ */\n", message_name);
    fprintf(cv_h_fptr,
        "\n#ifdef  __cplusplus\n}\n#endif\n\n#endif /* _%s_set_c_h_ */\n", message_name);
}

/**
 * Function to close standard generated files that will contain the source code for the
 * message;
 *
 * @param h_fptr header file containing the c structure of the message.
 * @param cv_h_fptr header file that will contain the declaration of convenience
 *        (de)serialization functions for the message.
 * @param cv_c_fptrm header file that will contain the implementation of convenience
 *        (de)serialization functions for the message.
 */
static void
close_standard_gen_files(FILE *h_fptr, FILE *cv_h_fptr, FILE *cv_c_fptr) {
    fclose(h_fptr);
    fclose(cv_h_fptr);
    fclose(cv_c_fptr);
}

/**
 * Generate source set from the provided string containing
 * cerializer dynamic message definition(s) in xml format.
 *
 * @param cerializer_dmd_xml string containing cerializer
 *        dynamic message definition(s) in xml format.
 */
static void
gen_source_set_from_string(char * cerializer_dmd_xml) {
    if (cerializer_dmd_xml != NULL) {
        ezxml_t cerializer_dmd =
            ezxml_parse_str(cerializer_dmd_xml, strlen(cerializer_dmd_xml));
        ezxml_t message, field;

        for (message = ezxml_child(cerializer_dmd, "message"); message; message = message->next) {
            FILE *h_fptr, *cv_h_fptr, *cv_c_fptr; /* set of generated files */
            message_info_struct message_info;
            char * message_name; /* name of message */
            int i;
            /* validate message name */
            exit_if_null(ezxml_attr(message, "name"), "unspecified message name attribute!");
            /* get a proper message name */
            message_name = get_c_proper_name(ezxml_attr(message, "name"));

            message_info.message_name = message_name;
            message_info.field_names = NULL;
            message_info.field_types = NULL;
            message_info.field_count = 0;
            /* open all files for writing */
            open_standard_gen_files(&h_fptr, &cv_h_fptr, &cv_c_fptr, message_name);
            /* prepare initially all files */
            prepare_standard_gen_files(h_fptr, cv_h_fptr, cv_c_fptr, message_name);
#ifdef DEBUG
            fprintf(stdout, "processing message %s\n", ezxml_attr(message, "name"));
#endif
            for (field = ezxml_child(message, "field"); field; field = field->next) {
                /* sanity checks */
                if (ezxml_attr(field, "name") == NULL || ezxml_txt(field) == NULL) {
                    continue;
                }
                if (!valid_field_value_type(ezxml_txt(field))) {
                    continue;
                }
                if (message_info.field_names == NULL) {
                    message_info.field_names = (char **)malloc(sizeof(char *));
                    exit_if_null(message_info.field_names, "unable to allocate enough memory!");
                } else {
                    char ** new_field_names = NULL;
                    new_field_names =
                        (char **)realloc(message_info.field_names, (message_info.field_count + 1)*sizeof(char *));
                    exit_if_null(new_field_names, "unable to re-allocate enough memory!");
                    message_info.field_names = new_field_names;
                }
                if (message_info.field_types == NULL) {
                    message_info.field_types = (char **)malloc(sizeof(char *));
                    exit_if_null(message_info.field_types, "unable to allocate enough memory!");
                } else {
                    char ** new_field_types = NULL;
                    new_field_types =
                        (char **)realloc(message_info.field_types, (message_info.field_count + 1)*sizeof(char *));
                    exit_if_null(new_field_types, "unable to re-llocate enough memory!");
                    message_info.field_types = new_field_types;
                }
#ifdef DEBUG
                fprintf(stdout, "adding field %s\n", ezxml_attr(field, "name"));
                fprintf(stdout, "    of type %s\n", ezxml_txt(field));
#endif
                message_info.field_names[message_info.field_count] = get_c_proper_name(ezxml_attr(field, "name"));
                message_info.field_types[message_info.field_count] = strdup(ezxml_txt(field));
                message_info.field_count++;
            }
            /* generate implementation source code */
            generate_implementation(h_fptr, cv_c_fptr, &message_info);
            if (message_info.field_count > 0) {
                /* free allocated memory resources */
                for (i=0; i<message_info.field_count; i++) {
                    free(message_info.field_names[i]);
                    free(message_info.field_types[i]);
                }
                free(message_info.field_names);
                free(message_info.field_types);
            }
            /* finalize at last all generated files */
            finalize_standard_gen_files(h_fptr, cv_h_fptr, cv_c_fptr, message_name);
            /* close all generate files */
            close_standard_gen_files(h_fptr, cv_h_fptr, cv_c_fptr);
            free(message_name);
        }
        ezxml_free(cerializer_dmd);
    }
}

/**
 * Generate source set from the provided file containing
 * cerializer dynamic message definition(s) in xml format.
 *
 * @param f_ptr the FILE * to use(not NULL).
 */
static void
generate_source_set_from_file(FILE * f_ptr) {
    if (f_ptr != NULL) {
        ezxml_t cerializer_dmd = ezxml_parse_fp(f_ptr);
        if (cerializer_dmd != NULL) {
            char * cerializer_dmd_xml_str;
#ifdef DEBUG
            ezxml_t message, field;
            fprintf(stdout, "%s\n", cerializer_dmd->name);

            for (message = ezxml_child(cerializer_dmd, "message"); message; message = message->next) {
                fprintf(stdout, "    message %s\n", ezxml_attr(message, "name"));
                for (field = ezxml_child(message, "field"); field; field = field->next) {
                    fprintf(stdout, "    field %s\n", ezxml_attr(field, "name"));
                    fprintf(stdout, "        type %s\n", ezxml_txt(field));
                }
            }
#endif
            cerializer_dmd_xml_str = ezxml_toxml(cerializer_dmd);
            gen_source_set_from_string(cerializer_dmd_xml_str);
            free(cerializer_dmd_xml_str);
            ezxml_free(cerializer_dmd);
        }
    }
}

/**
 * Print on screen instructions on how to use this tool.
 *
 * @param executable_name name of the tool.
 */
static void
print_usage(char * tool_name) {
    fprintf(stdout, "usage: %s -f <filename>\n", tool_name);
    fprintf(stdout, "(version: 1.0.1)\n");

    exit(1);
}

/**
 * Main function.
 */
int
main(int argc, char ** argv) {
#ifdef TEST
    test_gen_source_set(void);
#else
    int usage_error = 1;
    char * fname_ptr;
    FILE * f_ptr = NULL;
    /* validate command line arguments */
    if (argc == 3) {
        if (strcmp(argv[1], "-f") == 0) {
            fname_ptr = argv[2];
            usage_error = 0;
        }
    }

    if (usage_error) { /* if command line arguments not valid print usage*/
        print_usage(argv[0]);
    } else {
        if ((f_ptr = fopen(fname_ptr, "r")) == NULL) {
            fprintf(stderr, "%s [ERROR]: cannot open %s for reading\n",
                argv[1], fname_ptr);
        } else {
            generate_source_set_from_file(f_ptr);
            fclose(f_ptr);
        }
    }
#endif
    exit(0);
}

#ifdef TEST
/**
 * Function to test the implementation of cerializertool.
 */
static void
test_gen_source_set(void) {
    char * dmd_str;
    ezxml_t dmd = ezxml_new_d("crealized_dmd");
    ezxml_t message_1  = ezxml_add_child_d(dmd, "heartbeat", 0);
    ezxml_t message_1_field_1  = ezxml_add_child_d(message_1, "field", 0);
    ezxml_t message_1_field_2  = ezxml_add_child_d(message_1, "field", 1);
    ezxml_t message_1_field_3  = ezxml_add_child_d(message_1, "field", 2);

    ezxml_set_attr(message_1, "name", "my message");

    ezxml_set_attr(message_1_field_1, "name", "field 1");
    ezxml_set_txt_d(message_1_field_1, "INT32_TYPE");

    ezxml_set_attr(message_1_field_2, "name", "field 2");
    ezxml_set_txt_d(message_1_field_2, "FLOAT32_TYPE");

    ezxml_set_attr(message_1_field_3, "name", "field 3");
    ezxml_set_txt_d(message_1_field_3, "STRING_TYPE");
#ifdef DEBUG
    fprintf(stdout, "name = %s\n", dmd->name);
    fprintf(stdout, "xml string = %s\n", ezxml_toxml(dmd));
#endif
    dmd_str = ezxml_toxml(dmd);

    gen_source_set_from_string(dmd_str);

    free(dmd_str);
    ezxml_free(dmd);
}
#endif
