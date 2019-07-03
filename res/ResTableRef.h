/* 
 * File:   ResTableRef.h
 * Author : angelToms
 *
 */

#ifndef RESTABLEREF_H
#define	RESTABLEREF_H

#include <stdint.h>
/**
 *  This is a reference to a unique entry (a ResTable_entry structure)
 *  in a resource table.  The value is structured as: 0xpptteeee,
 *  where pp is the package index, tt is the type index in that
 *  package, and eeee is the entry index in that type.  The package
 *  and type values start at 1 for the first item, to help catch cases
 *  where they have not been supplied.
 */
struct ResTable_ref
{
    uint32_t ident;
};

#endif	/* RESTABLEREF_H */

