/* 
 * File:   SimResStringPool.h
 * Author : angelToms
 *
 */

#ifndef SIMRESSTRINGPOOL_H
#define	SIMRESSTRINGPOOL_H

#include "../../Types.h"
#include "../../res/ResStringPoolHeader.h"

#include <map>
#include <string>

class SimResStringPool {
public:
    SimResStringPool(u1* data, bool isCopy);
    virtual ~SimResStringPool();
    
    bool makeResStringPool();
    void printStringPool();
    const u1* getStringByEntryIndex(u4 index);
    const u1* getStyleByEntryIndex(u4 index);
    void mixupStringPool(std::map<std::string, std::string>& maps);

public:
    u1* mData;
    size_t mSize;
    bool mIsCopy;
    u1* mownData;

    struct ResStringPool_header* mStringPoolHeader;
    u1* mStrings;
    u1* mStyles;
    u4* mStringEntries;
    u4* mStyleEntries;
    u4 mStringCount;
    u4 mStyleCount;
    u4 mStringPoolSize;
    u4 mStylePoolSize;
};

#endif	/* SIMRESSTRINGPOOL_H */

