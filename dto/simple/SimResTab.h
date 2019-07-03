/* 
 * File:   SimResTab.h
 * Author : angelToms
 *
 */

#ifndef SIMRESTAB_H
#define	SIMRESTAB_H

#include "../../Types.h"

#include "SimResStringPool.h"
#include "SimResPackage.h"

#include <stdlib.h>
#include <stdio.h>
#include <vector>

class SimResTab {
public:
    SimResTab(u1* data, size_t size, bool isCopy);
    virtual ~SimResTab();
    
    bool makeSimResTab();
    void printAll(bool logAll, bool isAll);
    SimResStringPool* getSimResStringPool();

public:
    u1* mData;
    size_t mSize;
    bool mIsMakeable;
    bool mIsCopy;
    
    struct ResTable_header* mHeader;
    SimResStringPool* mStringPool;
    std::vector<SimResPackage*> mPackages;
     
};

#endif	/* SIMRESTAB_H */

