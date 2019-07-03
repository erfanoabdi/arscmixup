/* 
 * File:   SimResTypes.h
 * Author : angelToms
 *
 */

#ifndef SIMRESTYPES_H
#define	SIMRESTYPES_H

#include "SimResType.h"
#include "SimResTypeSpec.h"
#include "../../Types.h"

#include <vector>

class SimResTypes {
public:
    SimResTypes(u1 id, u4 entryCount);
    virtual ~SimResTypes();
    
    void add(SimResType* type);
    u4 getSize();
    u4 getEntryCount();

public:
    u1 mid;
    u4 mEntryCount;
    SimResTypeSpec* mTypeSpec;
    std::vector<SimResType*> mTypes;
};

#endif	/* SIMRESTYPES_H */

