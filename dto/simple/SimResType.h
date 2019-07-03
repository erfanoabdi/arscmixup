/* 
 * File:   SimResType.h
 * Author : angelToms
 *
 */

#ifndef SIMRESTYPE_H
#define	SIMRESTYPE_H

#include "../../res/ResTableType.h"
#include "../../Types.h"
#include "../../ResIncludes.h"

class SimResType {
public:
    SimResType(const ResTable_type* type, const u4 entryCount);
    virtual ~SimResType();
    
public:
    const ResTable_type* mType;
    const u4 mEntryCount;
    const u4* mEntries;//after ResTable_type

};

#endif	/* SIMRESTYPE_H */

