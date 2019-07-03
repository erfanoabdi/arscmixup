/* 
 * File:   SimResType.cpp
 * Author : angelToms
 * 
 */

#include "SimResType.h"

#include "../../QupLog.h"

SimResType::SimResType(const ResTable_type* type, const u4 entryCount) :
mType(type), mEntryCount(entryCount), mEntries(NULL) {
}

SimResType::~SimResType() {
}


