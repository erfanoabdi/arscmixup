/* 
 * File:   ResStringPoolRef.h
 * Author : angelToms
 *
 */

#ifndef RESSTRINGPOOLREF_H
#define	RESSTRINGPOOLREF_H

#include <stdint.h>

/**
 * Reference to a string in a string pool.
 */
struct ResStringPool_ref
{
    // Index into the string pool table (uint32_t-offset from the indices
    // immediately after ResStringPool_header) at which to find the location
    // of the string data in the pool.
    uint32_t index;
};

#endif	/* RESSTRINGPOOLREF_H */

