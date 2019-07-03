/* 
 * File:   ResTableHeader.h
 * Author : angelToms
 *
 */

#ifndef RESTABLEHEADER_H
#define	RESTABLEHEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ResChunkHead.h"
#include "../QupLog.h"

/**
 * Header for a resource table.  Its data contains a series of
 * additional chunks:
 *   * A ResStringPool_header containing all table values.  This string pool
 *     contains all of the string values in the entire resource table (not
 *     the names of entries or type identifiers however).
 *   * One or more ResTable_package chunks.
 *
 * Specific entries within a resource table can be uniquely identified
 * with a single integer as defined by the ResTable_ref structure.
 */
struct ResTable_header
{
    struct ResChunk_header header;

    // The number of ResTable_package structures.
    uint32_t packageCount;
};

extern "C"
{

__inline__ void printResTable(ResTable_header* restableHeader)
{
    QUP_LOGI("[*] package count = %d", restableHeader->packageCount);

    ResChunk_header* reschunkHeader = &restableHeader->header;
    printResChunkHeader(reschunkHeader);
}

}

#endif	/* RESTABLEHEADER_H */

