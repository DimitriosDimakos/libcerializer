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

#include "cerializer.h"
#include "stdlib_util.h"

/* All pack unpack methods taken by beej. */

/* macros for packing floats and doubles: */
#define pack754_16(f) (pack754((f), 16, 5))
#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_16(i) (unpack754((i), 16, 5))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

/**
 * Pack a floating point number to IEEE-754 format
 *
 * @param f floating point number to pack.
 * @param bits size of floating point number in bits.
 * @param expbits size of floating point number exponent in bits.
 *
 * @return integer value containing the packed floating point number,
 *          in IEEE-754 format.
 */
static unsigned long long int
pack754(long double f, unsigned bits, unsigned expbits) {
    long double fnorm;
    int shift;
    long long sign, exp, significand;
    unsigned significandbits = bits - expbits - 1; /* -1 for sign bit */

    if (f == 0.0) return 0; /* get this special case out of the way */

    /* check sign and begin normalization */
    if (f < 0) { sign = 1; fnorm = -f; }
    else { sign = 0; fnorm = f; }

    /* get the normalized form of f and track the exponent */
    shift = 0;
    while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
    while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
    fnorm = fnorm - 1.0;

    /* calculate the binary form (non-float) of the significant data */
    significand = fnorm * ((1LL<<significandbits) + 0.5f);

    /* get the biased exponent */
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

    /* return the final answer */
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

/**
 * Unpack a floating point number from IEEE-754 format
 *
 * @param i integer value containing the packed floating point number,
 *          in IEEE-754 format.
 * @param bits size of floating point number in bits.
 * @param expbits size of floating point number exponent in bits.
 *
 * @return unpacked floating point number.
 */
static long double
unpack754(unsigned long long int i, unsigned bits, unsigned expbits) {
    long double result;
    long long shift;
    unsigned bias;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (i == 0) return 0.0;

    /* pull the significant */
    result = (i&((1LL<<significandbits)-1)); /* mask */
    result /= (1LL<<significandbits); /* convert back to float */
    result += 1.0f; /* add the one back on */

    /* deal with the exponent */
    bias = (1<<(expbits-1)) - 1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
    while(shift > 0) { result *= 2.0; shift--; }
    while(shift < 0) { result /= 2.0; shift++; }

    /* sign it */
    result *= (i>>(bits-1))&1? -1.0: 1.0;

    return result;
}

/**
 * Store a 16-bit integer into a char buffer (like htonl()).
 *
 * @param buf unsigned char buffer buffer to store the 16-bit integer.
 * @param i 16-bit integer value.
 */
static void
packi16(unsigned char *buf, unsigned int i) {
    *buf++ = i>>8; *buf++ = i;
}

/**
 * Store a 32-bit integer into a char buffer (like htonl()).
 *
 * @param buf unsigned char buffer buffer to store the 32-bit integer.
 * @param i 32-bit integer value.
 */
static void
packi32(unsigned char *buf, unsigned long int i) {
    *buf++ = i>>24; *buf++ = i>>16;
    *buf++ = i>>8;  *buf++ = i;
}

/**
 * Store a 64-bit integer into a char buffer (like htonl()).
 *
 * @param buf unsigned char buffer to store the 64-bit integer.
 * @param i 64-bit integer value.
 */
static void
packi64(unsigned char *buf, unsigned long long int i) {
    *buf++ = i>>56; *buf++ = i>>48;
    *buf++ = i>>40; *buf++ = i>>32;
    *buf++ = i>>24; *buf++ = i>>16;
    *buf++ = i>>8;  *buf++ = i;
}

/**
 * Unpack a 16-bit integer from a char buffer (like ntohs()).
 *
 * @param buf unsigned char buffer representing a 16-bit integer.
 *
 * @return 16-bit integer value.
 */
static int
unpacki16(unsigned char *buf) {
    unsigned int i2 = ((unsigned int)buf[0]<<8) | buf[1];
    int i;

    /* change unsigned numbers to signed */
    if (i2 <= 0x7fffu) { i = i2; }
    else { i = -1 - (unsigned int)(0xffffu - i2); }

    return i;
}

/**
 * Unpack a 16-bit unsigned integer from a char buffer (like ntohs()).
 *
 * @param buf unsigned char buffer representing a 16-bit unsigned integer.
 *
 * @return 16-bit unsigned integer value.
 */
static unsigned int
unpacku16(unsigned char *buf) {
    return ((unsigned int)buf[0]<<8) | buf[1];
}

/**
 * Unpack a 32-bit integer from a char buffer (like ntohl()).
 *
 * @param buf unsigned char buffer representing a 32-bit integer.
 *
 * @return 32-bit integer value.
 */
static long int
unpacki32(unsigned char *buf) {
    unsigned long int i2 = ((unsigned long int)buf[0]<<24) |
                           ((unsigned long int)buf[1]<<16) |
                           ((unsigned long int)buf[2]<<8)  |
                           buf[3];
    long int i;

    /* change unsigned numbers to signed */
    if (i2 <= 0x7fffffffu) { i = i2; }
    else { i = -1 - (long int)(0xffffffffu - i2); }

    return i;
}

/**
 * Unpack a 32-bit unsigned integer from a char buffer (like ntohl()).
 *
 * @param buf unsigned char buffer representing a 32-bit unsigned integer.
 *
 * @return 32-bit unsigned integer value.
 */
static unsigned long int
unpacku32(unsigned char *buf) {
    return ((unsigned long int)buf[0]<<24) |
           ((unsigned long int)buf[1]<<16) |
           ((unsigned long int)buf[2]<<8)  |
           buf[3];
}

/**
 * Unpack a 64-bit integer from a char buffer (like ntohl()).
 *
 * @param buf unsigned char buffer representing a 64-bit integer.
 *
 * @return 64-bit integer value.
 */
static long long int
unpacki64(unsigned char *buf) {
    unsigned long long int i2 = ((unsigned long long int)buf[0]<<56) |
                                ((unsigned long long int)buf[1]<<48) |
                                ((unsigned long long int)buf[2]<<40) |
                                ((unsigned long long int)buf[3]<<32) |
                                ((unsigned long long int)buf[4]<<24) |
                                ((unsigned long long int)buf[5]<<16) |
                                ((unsigned long long int)buf[6]<<8)  |
                                buf[7];
    long long int i;

    /* change unsigned numbers to signed */
    if (i2 <= 0x7fffffffffffffffu) { i = i2; }
    else { i = -1 -(long long int)(0xffffffffffffffffu - i2); }

    return i;
}

/**
 * Unpack a 64-bit unsigned integer from a char buffer (like ntohl()).
 *
 * @param buf unsigned char buffer representing a 64-bit unsigned integer.
 *
 * @return 64-bit unsigned integer value.
 */
static unsigned long long int
unpacku64(unsigned char *buf) {
    return ((unsigned long long int)buf[0]<<56) |
           ((unsigned long long int)buf[1]<<48) |
           ((unsigned long long int)buf[2]<<40) |
           ((unsigned long long int)buf[3]<<32) |
           ((unsigned long long int)buf[4]<<24) |
           ((unsigned long long int)buf[5]<<16) |
           ((unsigned long long int)buf[6]<<8)  |
           buf[7];
}

/**
 * Copies a slice of the buffer pointed to by src, to the buffer pointed to by dest.
 *
 * @param dest destination buffer to store the src sliced buffer.
 * @param src origin buffer to retrieve the sliced data.
 *
 * @return reference to the destination buffer.
 */
extern unsigned char *
strslice(unsigned char *dest, const unsigned char *src, size_t start_idx, size_t n) {
    size_t i;
    size_t j = 0;
    for (i = start_idx ; i < (start_idx + n) ; i++) {
        dest[j++] = src[i];
    }
    return dest;
}

/**
 * Utility function to clear contents of a serialized_data_info structure;
 *
 * @param serdi serialized_data_info structure reference (not NULL).
 */
extern void
clear_serialized_data_info(serialized_data_info * serdi) {
    if (serdi != NULL) {
        if (serdi->ser_data != NULL) {
            SAFE_FREE(serdi->ser_data);
        }
        serdi->ser_data_len = 0;
    }
}

/**
 * Serialize a 16-bit integer into a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer to store the serialized 16-bit integer.
 * @param i 16-bit integer value.
 */
extern void
serialize_int16(unsigned char *buf, unsigned int i) {
    packi16(buf, i);
}

/**
 * De-serialize a 16-bit integer from a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer containing the serialized 16-bit integer.
 *
 * @return de-serialized 16-bit integer value.
 */
extern int
deserialize_int16(unsigned char *buf) {
    return (unpacki16(buf));
}

/**
 * De-serialize a 16-bit unsigned from a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer containing the serialized 16-bit unsigned integer.
 *
 * @return de-serialized 16-bit unsigned integer value.
 */
extern unsigned int
deserialize_uint16(unsigned char *buf) {
    return (unpacku16(buf));
}

/**
 * Serialize a 32-bit integer into a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer to store the serialized 32-bit integer.
 *
 * @param i 32-bit integer value.
 */
extern void
serialize_int32(unsigned char *buf, unsigned long int i) {
    packi32(buf, i);
}

/**
 * De-serialize a 32-bit integer from a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer containing the serialized 32-bit integer.
 *
 * @return de-serialized 32-bit integer value.
 */
extern long int
deserialize_int32(unsigned char *buf) {
    return (unpacki32(buf));
}

/**
 * De-serialize a 32-bit unsigned integer from a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer containing the serialized 32-bit unsigned integer.
 *
 * @return de-serialized 32-bit unsigned integer value.
 */
extern unsigned long int
deserialize_uint32(unsigned char *buf) {
    return(unpacku32(buf));
}

/**
 * Serialize a 64-bit integer into a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer to store the serialized 64-bit integer.
 * @param i 64-bit integer value.
 */
extern void
serialize_int64(unsigned char *buf, unsigned long long int i) {
    packi64(buf, i);
}

/**
 * De-serialize a 64-bit integer from a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer containing the serialized 64-bit unsigned integer.
 *
 * @return de-serialized 64-bit integer value.
 */
extern long long int
deserialize_int64(unsigned char *buf) {
    return (unpacki64(buf));
}

/**
 * De-serialize a 64-bit unsigned integer from a sequence of bytes buffer(big-endian version).
 *
 * @param buf sequence of bytes buffer containing the serialized 64-bit unsigned integer.
 *
 * @return de-serialized 64-bit unsigned integer value.
 */
extern unsigned long long int
deserialize_uint64(unsigned char *buf) {
    return (unpacku64(buf));
}


/**
 * Serialize a 32-bit floating point number(IEE 754 version).
 *
 * @param buf sequence of bytes buffer to store the
 *            serialized 32-bit floating point number.
 * @param f 32-bit floating point number value.
 */
extern void
serialize_float32(unsigned char *buf, float f) {
    unsigned long long int fhold = pack754_32(f); /* convert to IEEE 754 */
    packi32(buf, fhold); /* pack 32-bit integer */
}

/**
 * De-serialize a 32-bit floating point number from a sequence of bytes buffer(IEE 754 version).
 *
 * @param buf sequence of bytes buffer containing the serialized 32-bit floating point number.
 *
 * @return de-serialized 32-bit floating point number value.
 */
extern long double
deserialize_float32(unsigned char *buf) {
    unsigned long long int fhold = unpacku32(buf); /* unpack 32-bit integer */
    return (unpack754_32(fhold)); /* convert from IEE 754 */
}

/**
 * Serialize a 64-bit floating point number(IEE 754 version).
 *
 * @param buf sequence of bytes buffer to store the
 *            serialized 64-bit floating point number.
 * @param f 64-bit floating point number value.
 */
extern void
serialize_float64(unsigned char *buf, double f) {
    unsigned long long int fhold = pack754_64(f); /* convert to IEEE 754 */
    packi64(buf, fhold); /* pack 64-bit integer */
}

/**
 * De-serialize a 64-bit floating point number from a sequence of bytes buffer(IEE 754 version).
 *
 * @param buf sequence of bytes buffer containing the serialized 64-bit floating point number.
 *
 * @return de-serialized 64-bit floating point number value.
 */
extern double
deserialize_float64(unsigned char *buf) {
    unsigned long long int fhold = unpacku64(buf); /* unpack 64-bit integer */
    return ((double)unpack754_64(fhold)); /* convert from IEE 754 */
}
