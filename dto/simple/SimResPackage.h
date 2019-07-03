/* 
 * File:   SimResPackage.h
 * Author : angelToms
 *
 */

#ifndef SIMRESPACKAGE_H
#define	SIMRESPACKAGE_H

#include "../../res/ResTablePackage.h"
#include "../../Types.h"
#include "SimResStringPool.h"
#include "SimResTypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>

class SimResPackage {
public:
    SimResPackage(u1* data, bool isCopy, struct ResTable_header* header,
            SimResStringPool* stringPool);
    virtual ~SimResPackage();

    bool makeResPackage();
    bool makeTypes();
    bool makeResTypeSpec(const u1* chunk, const u1* endPos);
    bool makeResType(const u1* chunk, const u1* endPos);
    size_t getPkgSize();
    void printValue(const Res_value& value);
    void printEntryMap(const ResTable_entry* ent, size_t esize,
        int typeSize, const struct ResTable_map_entry* bagPtr);
    void printPkgString(bool isAll);
    

public:
    u1* mData;
    size_t mSize;
    bool mIsCopy;
    std::string mPkgName;

    struct ResTable_header* mHeader;
    struct ResTable_package* mResPackage;

    SimResStringPool* mStringPool;
    SimResStringPool* mTypeStrings;
    SimResStringPool* mKeyStrings;

    std::vector<SimResTypes*> mResTypes;

};

#endif	/* SIMRESPACKAGE_H */

