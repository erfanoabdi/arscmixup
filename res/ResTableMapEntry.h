/* 
 * File:   ResTableMapEntry.h
 * Author : angelToms
 *
 */

#ifndef RESTABLEMAPENTRY_H
#define	RESTABLEMAPENTRY_H

#include <stdint.h>

#include "ResTableRef.h"

#include "../QupLog.h"

/**
 * Extended form of a ResTable_entry for map entries, defining a parent map
 * resource from which to inherit values.
 */
struct ResTable_map_entry : public ResTable_entry {
    // Resource identifier of the parent mapping, or 0 if there is none.
    ResTable_ref parent;
    // Number of name/value pairs that follow for FLAG_COMPLEX.
    uint32_t count;
};

extern "C" {

    inline void printResTableMapEntry(const struct ResTable_map_entry* entry) {
        if (entry->parent.ident == 0) {
            QUP_LOGI("[*] ResTable_map_entry :\n"
                    "count = %d\n",
                    entry->count);
        } else {
            QUP_LOGI("[*] ResTable_map_entry :\n"
                    "parent ident = %d\n"
                    "count = %d\n",
                    entry->parent.ident,
                    entry->count);
        }
    }
}
#endif	/* RESTABLEMAPENTRY_H */

