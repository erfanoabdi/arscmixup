/* 
 * File:   SimResTypes.cpp
 * Author : angelToms
 * 
 */

#include "SimResTypes.h"

#include <stdlib.h>

SimResTypes::SimResTypes(u1 id, u4 entryCount) :
mid(id), mEntryCount(entryCount),  mTypeSpec(NULL) {
}

SimResTypes::~SimResTypes() {
    delete mTypeSpec;
    mTypeSpec = NULL;

    int size = mTypes.size();
    if (size > 0) {
        for (int i = 0; i < size; i++) {
            SimResType* type = mTypes[i];
            delete type;
            type = NULL;
        }
    }
    mTypes.clear();
}

void SimResTypes::add(SimResType* type) {
    if (type->mType->id != mid) {
        return;
    }
    mTypes.push_back(type);
}

u4 SimResTypes::getSize() {
    return mTypes.size();
}

u4 SimResTypes::getEntryCount() {
    return mEntryCount;
}