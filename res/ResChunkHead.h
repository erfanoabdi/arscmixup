/* 
 * File:   ResChunkHead.h
 * Author : angelToms
 *
 */

#ifndef RESCHUNKHEAD_H
#define	RESCHUNKHEAD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <string.h>

#include "../QupLog.h"

#define RES_MAX_LENGTH 256

typedef enum {
    RES_NULL_TYPE               = 0x0000,
    RES_STRING_POOL_TYPE        = 0x0001,
    RES_TABLE_TYPE              = 0x0002,
    RES_XML_TYPE                = 0x0003,

    // Chunk types in RES_XML_TYPE
    RES_XML_FIRST_CHUNK_TYPE    = 0x0100,
    RES_XML_START_NAMESPACE_TYPE= 0x0100,
    RES_XML_END_NAMESPACE_TYPE  = 0x0101,
    RES_XML_START_ELEMENT_TYPE  = 0x0102,
    RES_XML_END_ELEMENT_TYPE    = 0x0103,
    RES_XML_CDATA_TYPE          = 0x0104,
    RES_XML_LAST_CHUNK_TYPE     = 0x017f,
    // This contains a uint32_t array mapping strings in the string
    // pool back to resource identifiers.  It is optional.
    RES_XML_RESOURCE_MAP_TYPE   = 0x0180,

    // Chunk types in RES_TABLE_TYPE
    RES_TABLE_PACKAGE_TYPE      = 0x0200,
    RES_TABLE_TYPE_TYPE         = 0x0201,
    RES_TABLE_TYPE_SPEC_TYPE    = 0x0202
} ResChunkType;

/**
 * Macros for building/splitting resource identifiers.
 */
#define Res_VALIDID(resid) (resid != 0)
#define Res_CHECKID(resid) ((resid&0xFFFF0000) != 0)
#define Res_MAKEID(package, type, entry) \
    (((package+1)<<24) | (((type+1)&0xFF)<<16) | (entry&0xFFFF))
#define Res_GETPACKAGE(id) ((id>>24)-1)
#define Res_GETTYPE(id) (((id>>16)&0xFF)-1)
#define Res_GETENTRY(id) (id&0xFFFF)

#define Res_INTERNALID(resid) ((resid&0xFFFF0000) != 0 && (resid&0xFF0000) == 0)
#define Res_MAKEINTERNAL(entry) (0x01000000 | (entry&0xFFFF))
#define Res_MAKEARRAY(entry) (0x02000000 | (entry&0xFFFF))

#define Res_MAXPACKAGE 255

/** ********************************************************************
 *  Base Types
 *
 *  These are standard types that are shared between multiple specific
 *  resource types.
 *
 *********************************************************************** */

/**
 * Header that appears at the front of every data chunk in a resource.
 */
struct ResChunk_header
{
    // Type identifier for this chunk.  The meaning of this value depends
    // on the containing chunk.
    uint16_t type;

    // Size of the chunk header (in bytes).  Adding this value to
    // the address of the chunk allows you to find its associated data
    // (if any).
    uint16_t headerSize;

    // Total size of this chunk (in bytes).  This is the chunkSize plus
    // the size of any data associated with the chunk.  Adding this value
    // to the chunk allows you to completely skip its contents (including
    // any child chunks).  If this value is the same as chunkSize, there is
    // no data associated with the chunk.
    uint32_t size;
};

#ifdef __cplusplus
extern "C" {
#endif
    
__inline__ void getResChunkType(ResChunkType type, char* buf) {
    switch(type) {
        case RES_NULL_TYPE:
            memcpy(buf, "RES_NULL_TYPE", strlen("RES_NULL_TYPE"));
            break;
        case RES_STRING_POOL_TYPE:
            memcpy(buf, "RES_STRING_POOL_TYPE", strlen("RES_STRING_POOL_TYPE"));
            break;
        case RES_TABLE_TYPE:
            memcpy(buf, "RES_TABLE_TYPE", strlen("RES_TABLE_TYPE"));
            break;
        case RES_XML_TYPE:
            memcpy(buf, "RES_XML_TYPE", strlen("RES_XML_TYPE"));
            break;

        case RES_XML_FIRST_CHUNK_TYPE:
            memcpy(buf, "RES_XML_FIRST_CHUNK_TYPE | RES_XML_START_NAMESPACE_TYPE", 
                    strlen("RES_XML_FIRST_CHUNK_TYPE | RES_XML_START_NAMESPACE_TYPE"));
            break;
        case RES_XML_END_NAMESPACE_TYPE:
            memcpy(buf, "RES_XML_END_NAMESPACE_TYPE", strlen("RES_XML_END_NAMESPACE_TYPE"));
            break;
        case RES_XML_START_ELEMENT_TYPE:
            memcpy(buf, "RES_XML_START_ELEMENT_TYPE", strlen("RES_XML_START_ELEMENT_TYPE"));
            break;
        case RES_XML_END_ELEMENT_TYPE:
            memcpy(buf, "RES_XML_END_ELEMENT_TYPE", strlen("RES_XML_END_ELEMENT_TYPE"));
            break;
        case RES_XML_CDATA_TYPE:
            memcpy(buf, "RES_XML_CDATA_TYPE", strlen("RES_XML_CDATA_TYPE"));
            break;
        case RES_XML_LAST_CHUNK_TYPE:
            memcpy(buf, "RES_XML_LAST_CHUNK_TYPE", strlen("RES_XML_LAST_CHUNK_TYPE"));
            break;

        case RES_XML_RESOURCE_MAP_TYPE:
            memcpy(buf, "RES_XML_RESOURCE_MAP_TYPE", strlen("RES_XML_RESOURCE_MAP_TYPE"));
            break;
        case RES_TABLE_PACKAGE_TYPE:
            memcpy(buf, "RES_TABLE_PACKAGE_TYPE", strlen("RES_TABLE_PACKAGE_TYPE"));
            break;
        case RES_TABLE_TYPE_TYPE:
            memcpy(buf, "RES_TABLE_TYPE_TYPE", strlen("RES_TABLE_TYPE_TYPE"));
            break;
        case RES_TABLE_TYPE_SPEC_TYPE:
            memcpy(buf, "RES_TABLE_TYPE_SPEC_TYPE", strlen("RES_TABLE_TYPE_SPEC_TYPE"));
            break;
        default:
            memcpy(buf, "RES_TYPE_UNKNOW", strlen("RES_TYPE_UNKNOW"));
            break;
    }
}

__inline__ void printResChunkTypeString(ResChunkType type) {
    static char buf_[RES_MAX_LENGTH];
    memset(buf_, 0, RES_MAX_LENGTH);
    getResChunkType(type, buf_);
    QUP_LOGI("[*] res chunk type = %s", buf_);
}

__inline__ void printResChunkHeader(const struct ResChunk_header* header) {
    printResChunkTypeString((ResChunkType)header->type);
    QUP_LOGI("[*] ResChunk_header :\n[+] type = %x\n[+] head size = %u\n[+] size = %u", 
            header->type, header->headerSize, header->size);    
}
    
#ifdef __cplusplus  
}
#endif

#endif	/* RESCHUNKHEAD_H */

