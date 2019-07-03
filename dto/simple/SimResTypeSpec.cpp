/* 
 * File:   SimResTypeSpec.cpp
 * Author : angelToms
 * 
 */

#include "SimResTypeSpec.h"
#include "../../QupLog.h"

SimResTypeSpec::SimResTypeSpec(const size_t count, const ResTable_typeSpec* typeSpec) : mEntryCount(count),
mTypeSpec(typeSpec), mTypeSpecEntries(NULL) {
}

SimResTypeSpec::~SimResTypeSpec() {
}

void SimResTypeSpec::printTypeSpecConfig() {
    for(int i = 0 ; i < mEntryCount; i++) {
        const u4* typeSpecConfig = mTypeSpecEntries + i;
        QUP_LOGI("[*] ResTable_typeSpec[%d] config[%u] = 0x%08x",
                mTypeSpec->id,
                i,
                *typeSpecConfig);
    }
}

