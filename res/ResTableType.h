/* 
 * File:   ResTableType.h
 * Author : angelToms
 *
 */

#ifndef RESTABLETYPE_H
#define	RESTABLETYPE_H

#include <stdint.h>

#include "ResChunkHead.h"
#include "ResTableConfig.h"

/**
 * A collection of resource entries for a particular resource data
 * type. Followed by an array of uint32_t defining the resource
 * values, corresponding to the array of type strings in the
 * ResTable_package::typeStrings string block. Each of these hold an
 * index from entriesStart; a value of NO_ENTRY means that entry is
 * not defined.
 *
 * There may be multiple of these chunks for a particular resource type,
 * supply different configuration variations for the resource values of
 * that type.
 *
 * It would be nice to have an additional ordered index of entries, so
 * we can do a binary search if trying to find a resource by string name.
 */
struct ResTable_type
{
    struct ResChunk_header header;

    enum {
        NO_ENTRY = 0xFFFFFFFF
    };
    
    // The type identifier this chunk is holding.  Type IDs start
    // at 1 (corresponding to the value of the type bits in a
    // resource identifier).  0 is invalid.
    uint8_t id;
    
    // Must be 0.
    uint8_t res0;
    // Must be 0.
    uint16_t res1;
    
    // Number of uint32_t entry indices that follow.
    uint32_t entryCount;

    // Offset from header where ResTable_entry data starts.
    uint32_t entriesStart;
    
    // Configuration this collection of entries is designed for.
    ResTable_config config;
};

#ifdef __cplusplus
extern "C" {
#endif
    
    __inline__ void printResTabType(const struct ResTable_type* type) {
        printResChunkHeader(&type->header);
        
        QUP_LOGI("[*] ResTable_type :\n"
                "[+] id = %d\n"
                "[+] res0 = %d\n"
                "[+] res1 = %d\n"
                "[+] entryCount = %d\n"
                "[+] entriesStart = %d\n",
                type->id,
                type->res0,
                type->res1,
                type->entryCount,
                type->entriesStart);
    }
    
#ifdef __cplusplus
}
#endif

#endif	/* RESTABLETYPE_H */

