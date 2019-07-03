/* 
 * File:   ResStringPoolHeader.h
 * Author : angelToms
 *
 */

#ifndef RESSTRINGPOOLHEADER_H
#define	RESSTRINGPOOLHEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ResChunkHead.h"
#include "../QupLog.h"
#include "../Types.h"

enum {
    // If set, the string index is sorted by the string values (based
    // on strcmp16()).
    SORTED_FLAG = 1 << 0,

    // String pool is encoded in UTF-8
    UTF8_FLAG = 1 << 8
};

/** ********************************************************************
 *  String Pool
 *
 *  A set of strings that can be references by others through a
 *  ResStringPool_ref.
 *
 *********************************************************************** */

/**
 * Definition for a pool of strings.  The data of this chunk is an
 * array of uint32_t providing indices into the pool, relative to
 * stringsStart.  At stringsStart are all of the UTF-16 strings
 * concatenated together; each starts with a uint16_t of the string's
 * length and each ends with a 0x0000 terminator.  If a string is >
 * 32767 characters, the high bit of the length is set meaning to take
 * those 15 bits as a high word and it will be followed by another
 * uint16_t containing the low word.
 *
 * If styleCount is not zero, then immediately following the array of
 * uint32_t indices into the string table is another array of indices
 * into a style table starting at stylesStart.  Each entry in the
 * style table is an array of ResStringPool_span structures.
 */
struct ResStringPool_header {
    struct ResChunk_header header;

    // Number of strings in this pool (number of uint32_t indices that follow
    // in the data).
    uint32_t stringCount;

    // Number of style span arrays in the pool (number of uint32_t indices
    // follow the string indices).
    uint32_t styleCount;

    // Flags.
    uint32_t flags;

    // Index from header of the string data.
    uint32_t stringsStart;

    // Index from header of the style data.
    uint32_t stylesStart;
};

#ifdef __cplusplus
extern "C" {
#endif

    __inline__ void getResStringPoolHeaderFlags(u4 flags, char* buf) {
        if (flags & UTF8_FLAG)
            memcpy(buf, "UTF8_", strlen("UTF8_"));
        else
            memcpy(buf, "UTF16_", strlen("UTF16_"));
        
        if (flags & SORTED_FLAG)
            memcpy(buf + strlen(buf), "SORTED", strlen("SORTED"));
        else
            memcpy(buf + strlen(buf), "NOT_SORTED", strlen("NOT_SORTED"));
    }

    __inline__ void printResStringPoolFlagsString(u4 flags) {
        static char buf_[RES_MAX_LENGTH];
        memset(buf_, 0, RES_MAX_LENGTH);
        getResStringPoolHeaderFlags(flags, buf_);
        QUP_LOGI("[*] flags = %s", buf_);
    }

    __inline__ void printResStringPoolHeader(const struct ResStringPool_header* header) {
        printResChunkHeader(&header->header);
        QUP_LOGI("[*] ResStringPool_header :\n"
                "[+] stringCount = %u\n"
                "[+] flags = %u\n"
                "[+] styleCount = %u\n"
                "[+] stringsStart = %u\n"
                "[+] stylesStart = %u",
                header->stringCount, header->flags, header->styleCount, header->stringsStart,
                header->stylesStart);

        printResStringPoolFlagsString(header->flags);
    }

#ifdef __cplusplus  
}
#endif

#endif	/* RESSTRINGPOOLHEADER_H */

