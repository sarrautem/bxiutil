/* -*- coding: utf-8 -*-
 ###############################################################################
 # Author: Pierre Vigneras <pierre.vigneras@bull.net>
 # Created on: May 24, 2013
 # Contributors:
 ###############################################################################
 # Copyright (C) 2013  Bull S. A. S.  -  All rights reserved
 # Bull, Rue Jean Jaures, B.P.68, 78340, Les Clayes-sous-Bois
 # This is not Free or Open Source software.
 # Please contact Bull S. A. S. for details about its license.
 ###############################################################################
 */

#ifndef BXIMISC_H_
#define BXIMISC_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <error.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sysexits.h>

#include "bxi/base/err.h"
#include "bxi/base/time.h"
#include "bxi/base/log.h"


/**
 * @file    misc.h
 * @brief   Miscellaneous but useful functions.
 *
 * For bitarray manipulation
 * To declare an ``array'' of 47 bits:
 *
 *      char bitarray[BITNSLOTS(47)];
 *
 * To set the 23rd bit:
 *
 *      BITSET(bitarray, 23);
 *
 * To test the 35th bit:
 *
 *      if(BITTEST(bitarray, 35)) ...
 *
 * To compute the union of two bit arrays and place it in a third array
 * (with all three arrays declared as above):
 *
 *      for(i = 0; i < BITNSLOTS(47); i++)
 *          array3[i] = array1[i] | array2[i];
 *
 * To compute the intersection, use & instead of |.
 */


// *********************************************************************************
// ********************************** Defines **************************************
// *********************************************************************************



// Taken from c-faq.com: http://c-faq.com/misc/bitsets.html
#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= (char)BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= (char)~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)


/**
 * Return the MIN of two values
 */
#define BXIMISC_MIN(a,b)                                \
    ({                                          \
        (a) < (b) ? (a) : (b);                  \
    })

/**
 * Return the MAX of two values
 */
#define BXIMISC_MAX(a,b)                                \
    ({                                          \
        (a) > (b) ? (a) : (b);                  \
    })

/**
 * The error code returned when no digits are found in a given string.
 */
#define BXIMISC_NODIGITS_ERR 1917  // IGIT in leet speek

/**
 * The error code returned when an error happened while closing a file
 *
 */
#define BXIMISC_FILE_CLOSE_ERROR 1918

// *********************************************************************************
// ********************************** Types   **************************************
// *********************************************************************************

/**
 * The data structure used by `bximisc_stats()`.
 *
 * @see bximisc_stats()
 */
typedef struct {
    uint32_t min;               //!< the minimum value found
    uint32_t max;               //!< the maximum value found
    double mean;                //!< the mean value computed
    double stddev;              //!< the standard deviation computed
} bximisc_stats_s;

// *********************************************************************************
// ********************************** Global Variables *****************************
// *********************************************************************************

// *********************************************************************************
// ********************************** Interface ************************************
// *********************************************************************************

/**
 * Returns the filename of the given stream
 *
 * @param stream the stream to get the filename of
 * @return the name of the related file
 */
char * bximisc_get_filename(FILE * stream);

/**
 * Return the name of the file the given link_name is pointing to.
 *
 * The returned string will have to be freed().
 *
 * @param link_name the name of the link
 *
 * @return the name of the file the given `link_name` points to.
 */
char * bximisc_readlink(const char * link_name);

/**
 * Return a string representing the given tuple;
 *
 * Note: the tuple is read up to the endmark or until n values has been processed
 *
 * The returned string is allocated on the heap. It has to be freed using BXIFREE().
 *
 * @param n the maximum number of elements in the given tuple
 * @param tuple the tuple to get the string representation of
 * @param endmark a value representing an endmark
 * @param prefix the prefix character (can be '\0')
 * @param sep a separator character
 * @param suffix the suffix character (can be '\0\)
 *
 * @return a string representing the given tuple.

 */
char * bximisc_tuple_str(size_t n, const uint8_t * tuple, uint8_t endmark,
                         char prefix, char sep, char suffix);

/**
 * Create an array from a string description of the following format: (xx, yy, zz)
 *
 * Note: the given string is modified. The given result should be large enough to
 * hold all dimensions
 * Pre-condition: *start == '(', *end == ')'
 * Post-condition: dim < DIM_MAX
 *
 * @param start the starting pointer of the string to parse
 * @param end the end of the string (might not be '\0')
 * @param dim the number of element found
 * @param result a pointer on the resulting data
 * @return BXIERR_OK on success, anything else on error
 */
bxierr_p bximisc_str_tuple(const char * start, char * end,
                           const char prefix, const char sep, const char suffix,
                           uint8_t * const dim,
                           uint8_t * const result);


/**
 *  Computes the CRC-32 value of a memory buffer
 *
 *  Computes or accumulates the CRC-32 value for a memory buffer.
 *
 *  The `inCrc32` gives a previously accumulated CRC-32 value to allow
 *  a CRC to be generated for multiple sequential buffer-full of data.
 *
 *  The 'inCrc32' for the first buffer must be zero.
 *
 *
 *  @param inCrc32 accumulated CRC-32 value, must be 0 on first call
 *  @param buf     buffer to compute CRC-32 value for
 *  @param bufLen  number of bytes in buffer
 *
 *  @return computed CRC-32 value
 */
uint32_t bximisc_crc32(uint32_t inCrc32, const void *buf, size_t bufLen);

/**
 * Return the IP of the given hostname as a string.
 *
 * If the given host has several IPs, the first one is returned.
 *
 * @param hostname the hostname
 *
 * @return the IP of the given hostname
 */
char * bximisc_get_ip(char * hostname);


/**
 * Equivalent to strtoul but return a bxierr_p.
 *
 * @param str the string to parse
 * @param base the radix
 * @param result a pointer on the result
 *
 * @return BXIERR_OK on success, anything else on error
 *
 */
bxierr_p bximisc_strtoul(const char * str, int base, unsigned long *result);


/**
 *  Equivalent to strtol but return a bxierr_p.
 *
 * @param str the string to parse
 * @param base the radix
 * @param result a pointer on the result
 *
 * @return BXIERR_OK on success, anything else on error
 */
bxierr_p bximisc_strtol(const char * str, int base, long *result);

/**
 * Return a string representing the given bitarray.
 *
 * n represents the number of useful bit
 *
 * @param bitarray a bitarray
 * @param n the number of bits in the given bitarray
 *
 * @return a string representing the given bitarray
 */
char * bximisc_bitarray_str(const char *bitarray, uint64_t n);

/**
 * Return statistics on the given data.
 *
 * @param n the number of data
 * @param data the actual data to get statistics on
 * @param stats_p a pointer on a `bximisc_stats_s` data structure to be filled
 *
 */
#ifdef __cplusplus
void bximisc_stats(size_t n, uint32_t *data, bximisc_stats_s * stats_p);
#else
void bximisc_stats(size_t n, uint32_t data[n], bximisc_stats_s * stats_p);
#endif

/* --------------------------------------------------------------------------*/
/**
 * Map the file name filename on the process's memory.
 *
 * @param filename
 * @param size
 * @param exist
 * @param link_onfile
 * @param MMAP_PROT
 * @param addr
 *
 * @returns
 */
/* ----------------------------------------------------------------------------*/
bxierr_p bximisc_file_map(const char * filename,
                        size_t size, /**< Size of the file wanted */
                        bool exist, /**< define if the file existe or not*/
                        bool link_onfile, /**< Should the file link on the file*/
                        int MMAP_PROT, /**< flags provided to mmap for the page rights*/
                        char ** addr /**< return the addr of mapped file*/);

#endif /* BXIMISC_H_ */