/* 
 * File:   ResTabHeader.h
 * Author : angelToms
 *
 */

#ifndef RESTABHEADER_H
#define	RESTABHEADER_H

#include "ResStringPool.h"

#include "../ByteOrder.h"

#include <stdlib.h>

struct Header
{
    Header() :  ownedData(NULL), header(NULL),
        resourceIDMap(NULL), resourceIDMapSize(0) { }

    ~Header()
    {
        if(resourceIDMap)
            free(resourceIDMap);
    }

    void*                           ownedData;
    const ResTable_header*          header;
    size_t                          size;
    const uint8_t*                  dataEnd;
    size_t                          index;
    void*                           cookie;

    ResStringPool                   values;
    uint32_t*                       resourceIDMap;
    size_t                          resourceIDMapSize;
};

#ifdef __cplusplus
extern "C" {
#endif
    
    status_t validate_chunk(const ResChunk_header* chunk,
        size_t minSize,
        const uint8_t* dataEnd,
        const char* name);
    
#ifdef __cplusplus
}
#endif

#endif	/* RESTABHEADER_H */

