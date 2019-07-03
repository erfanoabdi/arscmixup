/* 
 * File:   ResTablePackage.h
 * Author : angelToms
 *
 */

#ifndef RESTABLEPACKAGE_H
#define	RESTABLEPACKAGE_H

#include <stdint.h>

#include "ResChunkHead.h"

typedef uint16_t char16_t;

/**
 * A collection of resource data types within a package.  Followed by
 * one or more ResTable_type and ResTable_typeSpec structures containing the
 * entry values for each resource type.
 */
struct ResTable_package {
    struct ResChunk_header header;

    // If this is a base package, its ID.  Package IDs start
    // at 1 (corresponding to the value of the package bits in a
    // resource identifier).  0 means this is not a base package.
    uint32_t id;

    // Actual name of this package, \0-terminated.
    char16_t name[128];

    // Offset to a ResStringPool_header defining the resource
    // type symbol table.  If zero, this package is inheriting from
    // another base package (overriding specific values in it).
    uint32_t typeStrings;

    // Last index into typeStrings that is for public use by others.
    uint32_t lastPublicType;

    // Offset to a ResStringPool_header defining the resource
    // key symbol table.  If zero, this package is inheriting from
    // another base package (overriding specific values in it).
    uint32_t keyStrings;

    // Last index into keyStrings that is for public use by others.
    uint32_t lastPublicKey;
};

extern "C" {

    __inline__ void printResTablePackage(ResTable_package* pkg, const char* name) {
        printResChunkHeader(&pkg->header);

        QUP_LOGI("[*] ResTable_package :\n"
                "[+] id = %d\n"
                "[+] name = %s\n"
                "[+] typeStrings = %d\n"
                "[+] lastPublicType = %d\n"
                "[+] keyStrings = %d\n"
                "[+] lastPublicKey = %d\n",
                pkg->id, 
                name,
                pkg->typeStrings,
                pkg->lastPublicType,
                pkg->keyStrings,
                pkg->lastPublicKey);
    }
}

#endif	/* RESTABLEPACKAGE_H */

