/* 
 * File:   ResTab.h
 * Author : angelToms
 *
 */

#ifndef RESTAB_H
#define	RESTAB_H

#include <stdlib.h>
#include <vector>
#include <stdint.h>

#include "../ResIncludes.h"
#include "../Types.h"
#include "../utils/String8.h"
#include "ResTabHeader.h"
#include "PackageGroup.h"
#include "ResStringPool.h"
#include "ResTabPackage.h"

#define CHAR16_TO_CSTR(c16, len) (String8(String16(c16,len)).string())

#define CHAR16_ARRAY_EQ(constant, var, len) \
        ((len == (sizeof(constant)/sizeof(constant[0]))) && (0 == memcmp((var), (constant), (len))))

struct resource_name {
    const char16_t* package;
    size_t packageLen;
    const char16_t* type;
    const char* type8;
    size_t typeLen;
    const char16_t* name;
    const char* name8;
    size_t nameLen;
};

class ResTab {
public:
    ResTab(u1* data, size_t len, bool isCopy);
    virtual ~ResTab();

    bool makeResTab();
    bool getResourceName(uint32_t resID, bool allowUtf8, resource_name* outName);
    void uninit();
    ssize_t getEntry(
            const Package* package, int typeIndex, int entryIndex,
            const ResTable_config* config,
            const ResTable_type** outType, const ResTable_entry** outEntry,
            const Type** outTypeClass);
    ssize_t getResourcePackageIndex(uint32_t resID);
    void print_value(const Package* pkg, const Res_value& value);
    void printResTab(bool logAll, bool incValues);
    status_t parsePackage(const ResTable_package * const pkg,
            const Header * const header, uint32_t idmap_id);

public:
    u1* mData;
    size_t mSize;
    bool mCopyData;

    //Sometimes one apk has one Header
    Header* mHeader;
    ResTable_config mParams;
    // Array of packages in all resource tables.
    std::vector<PackageGroup*> mPackageGroups;

    // Mapping from resource package IDs to indices into the internal
    // package array.
    uint8_t mPackageMap[256];
};

#endif	/* RESTAB_H */

