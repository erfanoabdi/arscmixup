/* 
 * File:   ResTableTypeSpec.h
 * Author : angelToms
 *
 */

#ifndef RESTABLETYPESPEC_H
#define	RESTABLETYPESPEC_H

#include <stdint.h>

#include "ResChunkHead.h"

/**
 * A specification of the resources defined by a particular type.
 *
 * There should be one of these chunks for each resource type.
 *
 * This structure is followed by an array of integers providing the set of
 * configuration change flags (ResTable_config::CONFIG_*) that have multiple
 * resources for that configuration.  In addition, the high bit is set if that
 * resource has been made public.
 */
struct ResTable_typeSpec
{
    struct ResChunk_header header;

    // The type identifier this chunk is holding.  Type IDs start
    // at 1 (corresponding to the value of the type bits in a
    // resource identifier).  0 is invalid.
    uint8_t id;
    
    // Must be 0.
    uint8_t res0;
    // Must be 0.
    uint16_t res1;
    
    // Number of uint32_t entry configuration masks that follow.
    uint32_t entryCount;

    enum {
        // Additional flag indicating an entry is public.
        SPEC_PUBLIC = 0x40000000
    };
};

#ifdef __cplusplus
extern "C" {
#endif
    
    __inline__ void printResTabTypeSepc(const struct ResTable_typeSpec* spec) {
        printResChunkHeader(&spec->header);
        
        QUP_LOGI("[*] ResTable_typeSpec :\n"
                "[+] id = %d\n"
                "[+] res0 = %d\n"
                "[+] res1 = %d\n"
                "[+] entryCount = %d\n",
                spec->id,
                spec->res0,
                spec->res1,
                spec->entryCount);
    }
    
#ifdef __cplusplus
}
#endif

#endif	/* RESTABLETYPESPEC_H */

