/* 
 * File:   SimResTypeSpec.h
 * Author : angelToms
 *
 */

#ifndef SIMRESTYPESPEC_H
#define	SIMRESTYPESPEC_H

#include <stdlib.h>
#include <vector>

#include "../../ResIncludes.h"
#include "../ResTabHeader.h"
#include "../../Types.h"

class SimResTypeSpec {
public:

    SimResTypeSpec(const size_t count, const ResTable_typeSpec* typeSpec);
    virtual ~SimResTypeSpec();
    
    void printTypeSpecConfig();

public:
    
    const size_t mEntryCount;
    const ResTable_typeSpec* mTypeSpec;
    const u4* mTypeSpecEntries;
};

#endif	/* SIMRESTYPESPEC_H */

