/* 
 * File:   ResTableEntry.h
 * Author : angelToms
 *
 */

#ifndef RESTABLEENTRY_H
#define	RESTABLEENTRY_H

#include <stdint.h>

#include "ResStringPoolHeader.h"
#include "ResStringPoolRef.h"
/**
 * This is the beginning of information about an entry in the resource
 * table.  It holds the reference to the name of this entry, and is
 * immediately followed by one of:
 *   * A Res_value structure, if FLAG_COMPLEX is -not- set.
 *   * An array of ResTable_map structures, if FLAG_COMPLEX is set.
 *     These supply a set of name/value mappings of data.
 */
struct ResTable_entry
{
    // Number of bytes in this structure.
    uint16_t size;

    enum {
        // If set, this is a complex entry, holding a set of name/value
        // mappings.  It is followed by an array of ResTable_map structures.
        FLAG_COMPLEX = 0x0001,
        // If set, this resource has been declared public, so libraries
        // are allowed to reference it.
        FLAG_PUBLIC = 0x0002
    };
    uint16_t flags;
    
    // Reference into ResTable_package::keyStrings identifying this entry.
    struct ResStringPool_ref key;
};

extern "C" {
    __inline__ void printResTableEntry(const ResTable_entry* entry) {
        const char* type = (entry->flags & ResTable_entry::FLAG_COMPLEX) ? "ResTable_map_entry" : "Res_value";
        const char* isPub = (entry->flags & ResTable_entry::FLAG_PUBLIC) ? "Public" : "Not Public";
        QUP_LOGI("[*] ResTable_entry :\n"
                "[+] size = %d\n"
                "[+] flags = 0x%08x\n"
                "[+] ResStringPool_ref index = %d\n"
                "[+] entry type = %s-%s",
                entry->size,
                entry->flags,
                entry->key.index,
                isPub,
                type);
    }
}
#endif	/* RESTABLEENTRY_H */

