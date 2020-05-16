/* 
 * File:   ResTab.cpp
 * Author : angelToms
 * 
 */

#include "ResTab.h"
#include "../utils/Unicode.h"

#include <string.h>

ResTab::ResTab(u1* data, size_t len, bool isCopy) :
mData(data), mSize(len), mCopyData(isCopy), mHeader(NULL) {
    memset(&mParams, 0, sizeof (mParams));
    memset(mPackageMap, 0, sizeof (mPackageMap));
}

ResTab::~ResTab() {
    uninit();
}

void ResTab::uninit() {
    size_t N = mPackageGroups.size();
    for (size_t i = 0; i < N; i++) {
        PackageGroup* g = mPackageGroups[i];
        delete g;
    }

    Header* header = mHeader;
    if (header->ownedData) {
        free(header->ownedData);
    }
    delete header;

    mPackageGroups.clear();
}

bool ResTab::makeResTab() {
    u1* p = mData;
    mHeader = new Header();
    if (!mHeader) {
        QUP_LOGI("[-] alloc dto header fail");
        return false;
    }

    mHeader->index = 1;
    mHeader->cookie = (void*) mData;

    const bool notDeviceEndian = htods(0xf0) != 0xf0;

    if (mCopyData || notDeviceEndian) {
        mHeader->ownedData = malloc(mSize);
        if (mHeader->ownedData == NULL) {
            return false;
        }
        memcpy(mHeader->ownedData, mData, mSize);
        p = (u1*) mHeader->ownedData;
    }

    mHeader->header = (const ResTable_header*) p;
    mHeader->size = dtohl(mHeader->header->header.size);
    QUP_LOGI("[*] total size of this chunk (in bytes) = %u", mHeader->size);

#ifdef DEBUG_ALL
    printHexData(2, header->header, header->size < 256 ? header->size : 256,
            16, 16, 0, false, NULL);
#endif

    if (dtohs(mHeader->header->header.headerSize) > mHeader->size
            || mHeader->size > mSize) {
        QUP_LOGI("[-] bad resource table: header size 0x%x or total size 0x%x is larger than data size 0x%x\n",
                (int) dtohs(mHeader->header->header.headerSize),
                (int) mHeader->size, (int) mSize);
        return false;
    }

    if (((dtohs(mHeader->header->header.headerSize) | mHeader->size)&0x3) != 0) {
        QUP_LOGI("[-] bad resource table: header size 0x%x or total size 0x%x is not on an integer boundary\n",
                (int) dtohs(mHeader->header->header.headerSize),
                (int) mHeader->size);
        return false;
    }

    mHeader->dataEnd = ((const u1*) mHeader->header) + mHeader->size;

    // Iterate through all chunks.
    size_t curPackage = 0;
    //over ResTableHeader
    const ResChunk_header* chunk = (const ResChunk_header*) (((const uint8_t*) mHeader->header)
            + dtohs(mHeader->header->header.headerSize));

    while (((const uint8_t*) chunk) <= (mHeader->dataEnd - sizeof (ResChunk_header)) &&
            ((const uint8_t*) chunk) <= (mHeader->dataEnd - dtohl(chunk->size))) {
        status_t err = validate_chunk(chunk, sizeof (ResChunk_header), mHeader->dataEnd, "ResTable");
        if (err != NO_ERROR) {
            return false;
        }

        QUP_LOGI("[*] chunk: type=0x%x, headerSize=0x%x, size=0x%x, pos=%p\n",
                dtohs(chunk->type), dtohs(chunk->headerSize), dtohl(chunk->size),
                (void*) (((const uint8_t*) chunk) - ((const uint8_t*) mHeader->header)));

        printResChunkHeader(chunk);

        const size_t csize = dtohl(chunk->size);
        const uint16_t ctype = dtohs(chunk->type);

        if (ctype == RES_STRING_POOL_TYPE) {
            if (mHeader->values.getError() != NO_ERROR) {
                // Only use the first string chunk; ignore any others that
                // may appear.
                status_t err = mHeader->values.setTo(chunk, csize);
                if (err != NO_ERROR) {
                    QUP_LOGI("[-] call set to ret error");
                    return false;
                }
            } else {
                QUP_LOGI("[*] multiple string chunks found in resource table.");
            }
        } else if (ctype == RES_TABLE_PACKAGE_TYPE) {
            if (curPackage >= dtohl(mHeader->header->packageCount)) {
                QUP_LOGI("[-] more package chunks were found than the %d declared in the header.",
                        dtohl(mHeader->header->packageCount));
                return false;
            }
            uint32_t idmap_id = 0;

            if (parsePackage((ResTable_package*) chunk, mHeader, idmap_id) != NO_ERROR) {
                QUP_LOGI("[-] parse package ret error");
                return false;
            }
            curPackage++;
        } else {
            QUP_LOGI("[*] unknown chunk type %p in table at %p.\n",
                    (void*) (int) (ctype),
                    (void*) (((const uint8_t*) chunk) - ((const uint8_t*) mHeader->header)));
        }

        //下一个结构内嵌的ResChunk_header
        chunk = (const ResChunk_header*)
                (((const uint8_t*) chunk) + csize);

    }

    return true;
}

ssize_t ResTab::getResourcePackageIndex(uint32_t resID) {
    return ((ssize_t) mPackageMap[Res_GETPACKAGE(resID) + 1]) - 1;
}

ssize_t ResTab::getEntry(
        const Package* package, int typeIndex, int entryIndex,
        const ResTable_config* config,
        const ResTable_type** outType, const ResTable_entry** outEntry,
        const Type** outTypeClass) {
    QUP_LOGI("[*] Getting entry from package %p\n", package);
    const ResTable_package * const pkg = package->package;

    const Type* allTypes = package->getType(typeIndex);
    QUP_LOGI("[*] allTypes=%p\n", allTypes);
    if (allTypes == NULL) {
        QUP_LOGI("[-] Skipping entry type index 0x%02x because type is NULL!\n", typeIndex);
        return 0;
    }

    if ((size_t) entryIndex >= allTypes->entryCount) {
        QUP_LOGI("[-] getEntry failing because entryIndex %d is beyond type entryCount %d",
                entryIndex, (int) allTypes->entryCount);
        return BAD_TYPE;
    }

    const ResTable_type* type = NULL;
    uint32_t offset = ResTable_type::NO_ENTRY;
    ResTable_config bestConfig;
    memset(&bestConfig, 0, sizeof (bestConfig)); // make the compiler shut up

    const size_t NT = allTypes->configs.size();
    for (size_t i = 0; i < NT; i++) {
        const ResTable_type * const thisType = allTypes->configs[i];
        if (thisType == NULL) continue;

        ResTable_config thisConfig;
        copyFromDtoH(thisType->config, thisConfig);

        TABLE_GETENTRY(QUP_LOGI("[*] Match entry 0x%x in type 0x%x (sz 0x%x): %s\n",
                entryIndex, typeIndex + 1, dtohl(thisType->config.size),
                thisConfig.toString().string()));

        // Check to make sure this one is valid for the current parameters.
        if (config && !matchConfig(*config, thisConfig)) {
            TABLE_GETENTRY(QUP_LOGI("[*] Does not match config!\n"));
            continue;
        }

        // Check if there is the desired entry in this type.

        const uint8_t * const end = ((const uint8_t*) thisType)
                + dtohl(thisType->header.size);
        const uint32_t * const eindex = (const uint32_t*)
                (((const uint8_t*) thisType) + dtohs(thisType->header.headerSize));

        uint32_t thisOffset = dtohl(eindex[entryIndex]);
        if (thisOffset == ResTable_type::NO_ENTRY) {
            TABLE_GETENTRY(QUP_LOGI("[*] Skipping because it is not defined!\n"));
            continue;
        }

        if (type != NULL) {
            // Check if this one is less specific than the last found.  If so,
            // we will skip it.  We check starting with things we most care
            // about to those we least care about.
            if (!isBetterThan(bestConfig, config, thisConfig)) {
                TABLE_GETENTRY(QUP_LOGI("[*] This config is worse than last!\n"));
                continue;
            }
        }

        type = thisType;
        offset = thisOffset;
        bestConfig = thisConfig;
        TABLE_GETENTRY(QUP_LOGI("[*] Best entry so far -- using it!\n"));
        if (!config) break;
    }

    if (type == NULL) {
        TABLE_GETENTRY(QUP_LOGI("[-] No value found for requested entry!\n"));
        return BAD_INDEX;
    }

    offset += dtohl(type->entriesStart);
    TABLE_NOISY(aout << "Looking in resource table " << package->header->header
            << ", typeOff="
            << (void*) (((const char*) type)-((const char*) package->header->header))
            << ", offset=" << (void*) offset << endl);

    if (offset > (dtohl(type->header.size) - sizeof (ResTable_entry))) {
        QUP_LOGI("[-] ResTable_entry at 0x%x is beyond type chunk data 0x%x",
                offset, dtohl(type->header.size));
        return BAD_TYPE;
    }
    if ((offset & 0x3) != 0) {
        QUP_LOGI("[-] ResTable_entry at 0x%x is not on an integer boundary",
                offset);
        return BAD_TYPE;
    }

    const ResTable_entry * const entry = (const ResTable_entry*)
            (((const uint8_t*) type) + offset);
    if (dtohs(entry->size) < sizeof (*entry)) {
        QUP_LOGI("[-] ResTable_entry size 0x%x is too small", dtohs(entry->size));
        return BAD_TYPE;
    }

    *outType = type;
    *outEntry = entry;
    if (outTypeClass != NULL) {
        *outTypeClass = allTypes;
    }
    return offset + dtohs(entry->size);
}

bool ResTab::getResourceName(uint32_t resID, bool allowUtf8, resource_name* outName) {


    const ssize_t p = getResourcePackageIndex(resID);
    const int t = Res_GETTYPE(resID);
    const int e = Res_GETENTRY(resID);

    if (p < 0) {
        if (Res_GETPACKAGE(resID) + 1 == 0) {
            QUP_LOGI("[-] No package identifier when getting name for resource number 0x%08x", resID);
        } else {
            QUP_LOGI("[-] No known package when getting name for resource number 0x%08x", resID);
        }
        return false;
    }
    if (t < 0) {
        QUP_LOGI("[-] No type identifier when getting name for resource number 0x%08x", resID);
        return false;
    }

    const PackageGroup * const grp = mPackageGroups[p];
    if (grp == NULL) {
        QUP_LOGI("[-] Bad identifier when getting name for resource number 0x%08x", resID);
        return false;
    }
    if (grp->packages.size() > 0) {
        const Package * const package = grp->packages[0];

        const ResTable_type* type;
        const ResTable_entry* entry;
        ssize_t offset = getEntry(package, t, e, NULL, &type, &entry, NULL);
        if (offset <= 0) {
            return false;
        }

        outName->package = grp->name.string();
        outName->packageLen = grp->name.size();
        if (allowUtf8) {
            outName->type8 = grp->basePackage->typeStrings.string8At(t, &outName->typeLen);
            outName->name8 = grp->basePackage->keyStrings.string8At(
                    dtohl(entry->key.index), &outName->nameLen);
        } else {
            outName->type8 = NULL;
            outName->name8 = NULL;
        }
        if (outName->type8 == NULL) {
            outName->type = grp->basePackage->typeStrings.stringAt(t, &outName->typeLen);
            // If we have a bad index for some reason, we should abort.
            if (outName->type == NULL) {
                return false;
            }
        }
        if (outName->name8 == NULL) {
            outName->name = grp->basePackage->keyStrings.stringAt(
                    dtohl(entry->key.index), &outName->nameLen);
            // If we have a bad index for some reason, we should abort.
            if (outName->name == NULL) {
                return false;
            }
        }

        return true;
    }

    return false;
}

void ResTab::print_value(const Package* pkg, const Res_value& value) {
    if (value.dataType == Res_value::TYPE_NULL) {
        printf("(null)\n");
    } else if (value.dataType == Res_value::TYPE_REFERENCE) {
        printf("(reference) 0x%08x\n", value.data);
    } else if (value.dataType == Res_value::TYPE_ATTRIBUTE) {
        printf("(attribute) 0x%08x\n", value.data);
    } else if (value.dataType == Res_value::TYPE_STRING) {
        size_t len;
        const char* str8 = pkg->header->values.string8At(
                value.data, &len);
        if (str8 != NULL) {
            printf("(string8) \"%s\"\n", String8::normalizeForOutput(str8).string());
        } else {
            const uint16_t* str16 = pkg->header->values.stringAt(
                    value.data, &len);
            if (str16 != NULL) {
                printf("(string16) \"%s\"\n",
                        String8::normalizeForOutput(String8(str16, len).string()).string());
            } else {
                printf("(string) null\n");
            }
        }
    } else if (value.dataType == Res_value::TYPE_FLOAT) {
        printf("(float) %g\n", *(const float*) &value.data);
    } else if (value.dataType == Res_value::TYPE_DIMENSION) {
        printf("(dimension) ");
        print_complex(value.data, false);
        printf("\n");
    } else if (value.dataType == Res_value::TYPE_FRACTION) {
        printf("(fraction) ");
        print_complex(value.data, true);
        printf("\n");
    } else if (value.dataType >= Res_value::TYPE_FIRST_COLOR_INT
            || value.dataType <= Res_value::TYPE_LAST_COLOR_INT) {
        printf("(color) #%08x\n", value.data);
    } else if (value.dataType == Res_value::TYPE_INT_BOOLEAN) {
        printf("(boolean) %s\n", value.data ? "true" : "false");
    } else if (value.dataType >= Res_value::TYPE_FIRST_INT
            || value.dataType <= Res_value::TYPE_LAST_INT) {
        printf("(int) 0x%08x or %d\n", value.data, value.data);
    } else {
        printf("(unknown type) t=0x%02x d=0x%08x (s=0x%04x r=0x%02x)\n",
                (int) value.dataType, (int) value.data,
                (int) value.size, (int) value.res0);
    }
}

void ResTab::printResTab(bool logAll, bool inclValues) {
    if (logAll) {
#if 0
        printf("mParams=%c%c-%c%c,\n",
                mParams.language[0], mParams.language[1],
                mParams.country[0], mParams.country[1]);
#endif
        size_t pgCount = mPackageGroups.size();
        printf("Package Groups (%d)\n", (int) pgCount);
        for (size_t pgIndex = 0; pgIndex < pgCount; pgIndex++) {
            const PackageGroup* pg = mPackageGroups[pgIndex];
            printf("Package Group %d id=%d packageCount=%d name=%s\n",
                    (int) pgIndex, pg->id, (int) pg->packages.size(),
                    String8(pg->name).string());

            size_t pkgCount = pg->packages.size();
            for (size_t pkgIndex = 0; pkgIndex < pkgCount; pkgIndex++) {
                const Package* pkg = pg->packages[pkgIndex];
                size_t typeCount = pkg->types.size();
                printf("  Package %d id=%d name=%s typeCount=%d\n", (int) pkgIndex,
                        pkg->package->id, String8(String16(pkg->package->name)).string(),
                        (int) typeCount);
                for (size_t typeIndex = 0; typeIndex < typeCount; typeIndex++) {
                    const Type* typeConfigs = pkg->getType(typeIndex);
                    if (typeConfigs == NULL) {
                        printf("    type %d NULL\n", (int) typeIndex);
                        continue;
                    }
                    const size_t NTC = typeConfigs->configs.size();
                    printf("    type %d configCount=%d entryCount=%d\n",
                            (int) typeIndex, (int) NTC, (int) typeConfigs->entryCount);
                    if (typeConfigs->typeSpecFlags != NULL) {
                        for (size_t entryIndex = 0; entryIndex < typeConfigs->entryCount; entryIndex++) {
                            uint32_t resID = (0xff000000 & ((pkg->package->id) << 24))
                                    | (0x00ff0000 & ((typeIndex + 1) << 16))
                                    | (0x0000ffff & (entryIndex));
                            resource_name resName;
                            if (this->getResourceName(resID, true, &resName)) {
                                String8 type8;
                                String8 name8;
                                if (resName.type8 != NULL) {
                                    type8 = String8(resName.type8, resName.typeLen);
                                } else {
                                    type8 = String8(resName.type, resName.typeLen);
                                }
                                if (resName.name8 != NULL) {
                                    name8 = String8(resName.name8, resName.nameLen);
                                } else {
                                    name8 = String8(resName.name, resName.nameLen);
                                }
                                printf("      spec resource 0x%08x %s:%s/%s: flags=0x%08x\n",
                                        resID,
                                        uint16_tO_CSTR(resName.package, resName.packageLen),
                                        type8.string(), name8.string(),
                                        dtohl(typeConfigs->typeSpecFlags[entryIndex]));
                            } else {
                                printf("      INVALID TYPE CONFIG FOR RESOURCE 0x%08x\n", resID);
                            }
                        }
                    }
                    for (size_t configIndex = 0; configIndex < NTC; configIndex++) {
                        const ResTable_type* type = typeConfigs->configs[configIndex];
                        if ((((uint64_t) type)&0x3) != 0) {
                            printf("      NON-INTEGER ResTable_type ADDRESS: %p\n", type);
                            continue;
                        }
                        String8 configStr = configToString(type->config);
                        printf("      config %s:\n", configStr.size() > 0
                                ? configStr.string() : "(default)");
                        size_t entryCount = dtohl(type->entryCount);
                        uint32_t entriesStart = dtohl(type->entriesStart);
                        if ((entriesStart & 0x3) != 0) {
                            printf("      NON-INTEGER ResTable_type entriesStart OFFSET: %p\n", (void*) entriesStart);
                            continue;
                        }
                        uint32_t typeSize = dtohl(type->header.size);
                        if ((typeSize & 0x3) != 0) {
                            printf("      NON-INTEGER ResTable_type header.size: %p\n", (void*) typeSize);
                            continue;
                        }
                        for (size_t entryIndex = 0; entryIndex < entryCount; entryIndex++) {

                            const uint8_t * const end = ((const uint8_t*) type)
                                    + dtohl(type->header.size);
                            const uint32_t * const eindex = (const uint32_t*)
                                    (((const uint8_t*) type) + dtohs(type->header.headerSize));

                            uint32_t thisOffset = dtohl(eindex[entryIndex]);
                            if (thisOffset == ResTable_type::NO_ENTRY) {
                                continue;
                            }

                            uint32_t resID = (0xff000000 & ((pkg->package->id) << 24))
                                    | (0x00ff0000 & ((typeIndex + 1) << 16))
                                    | (0x0000ffff & (entryIndex));
                            resource_name resName;
                            if (this->getResourceName(resID, true, &resName)) {
                                String8 type8;
                                String8 name8;
                                if (resName.type8 != NULL) {
                                    type8 = String8(resName.type8, resName.typeLen);
                                } else {
                                    type8 = String8(resName.type, resName.typeLen);
                                }
                                if (resName.name8 != NULL) {
                                    name8 = String8(resName.name8, resName.nameLen);
                                } else {
                                    name8 = String8(resName.name, resName.nameLen);
                                }
                                printf("        resource 0x%08x %s:%s/%s: ", resID,
                                        uint16_tO_CSTR(resName.package, resName.packageLen),
                                        type8.string(), name8.string());
                            } else {
                                printf("        INVALID RESOURCE 0x%08x: ", resID);
                            }
                            if ((thisOffset & 0x3) != 0) {
                                printf("NON-INTEGER OFFSET: %p\n", (void*) thisOffset);
                                continue;
                            }
                            if ((thisOffset + sizeof (ResTable_entry)) > typeSize) {
                                printf("OFFSET OUT OF BOUNDS: %p+%p (size is %p)\n",
                                        (void*) entriesStart, (void*) thisOffset,
                                        (void*) typeSize);
                                continue;
                            }

                            const ResTable_entry* ent = (const ResTable_entry*)
                                    (((const uint8_t*) type) + entriesStart + thisOffset);
                            if (((entriesStart + thisOffset)&0x3) != 0) {
                                printf("NON-INTEGER ResTable_entry OFFSET: %p\n",
                                        (void*) (entriesStart + thisOffset));
                                continue;
                            }

                            uint16_t esize = dtohs(ent->size);
                            if ((esize & 0x3) != 0) {
                                printf("NON-INTEGER ResTable_entry SIZE: %p\n", (void*) esize);
                                continue;
                            }
                            if ((thisOffset + esize) > typeSize) {
                                printf("ResTable_entry OUT OF BOUNDS: %p+%p+%p (size is %p)\n",
                                        (void*) entriesStart, (void*) thisOffset,
                                        (void*) esize, (void*) typeSize);
                                continue;
                            }

                            const Res_value* valuePtr = NULL;
                            const ResTable_map_entry* bagPtr = NULL;
                            Res_value value;
                            if ((dtohs(ent->flags) & ResTable_entry::FLAG_COMPLEX) != 0) {
                                printf("<bag>");
                                bagPtr = (const ResTable_map_entry*) ent;
                            } else {
                                valuePtr = (const Res_value*)
                                        (((const uint8_t*) ent) + esize);
                                copyFrom_dtoh(*valuePtr, value);
                                printf("t=0x%02x d=0x%08x (s=0x%04x r=0x%02x)",
                                        (int) value.dataType, (int) value.data,
                                        (int) value.size, (int) value.res0);
                            }

                            if ((dtohs(ent->flags) & ResTable_entry::FLAG_PUBLIC) != 0) {
                                printf(" (PUBLIC)");
                            }
                            printf("\n");

                            if (inclValues) {
                                if (valuePtr != NULL) {
                                    printf("          ");
                                    print_value(pkg, value);
                                } else if (bagPtr != NULL) {
                                    const int N = dtohl(bagPtr->count);
                                    const uint8_t* baseMapPtr = (const uint8_t*) ent;
                                    size_t mapOffset = esize;
                                    const ResTable_map* mapPtr = (ResTable_map*) (baseMapPtr + mapOffset);
                                    printf("          Parent=0x%08x, Count=%d\n",
                                            dtohl(bagPtr->parent.ident), N);
                                    for (int i = 0; i < N && mapOffset < (typeSize - sizeof (ResTable_map)); i++) {
                                        printf("          #%i (Key=0x%08x): ",
                                                i, dtohl(mapPtr->name.ident));
                                        copyFrom_dtoh(mapPtr->value, value);
                                        print_value(pkg, value);
                                        const size_t size = dtohs(mapPtr->value.size);
                                        mapOffset += size + sizeof (*mapPtr) - sizeof (mapPtr->value);
                                        mapPtr = (ResTable_map*) (baseMapPtr + mapOffset);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// Standard C isspace() is only required to look at the low byte of its input, so
// produces incorrect results for UTF-16 characters.  For safety's sake, assume that
// any high-byte UTF-16 code point is not whitespace.

inline int isspace16(uint16_t c) {
    return (c < 0x0080 && isspace(c));
}

status_t ResTab::parsePackage(const ResTable_package * const pkg,
        const Header * const header, uint32_t idmap_id) {
    const uint8_t* base = (const uint8_t*) pkg;
    status_t err = validate_chunk(&pkg->header, sizeof (*pkg),
            header->dataEnd, "ResTable_package");
    if (err != NO_ERROR) {
        return err;
    }

    const size_t pkgSize = dtohl(pkg->header.size);

    if (dtohl(pkg->typeStrings) >= pkgSize) {
        QUP_LOGI("[-] resTable_package type strings at %p are past chunk size %p.",
                (void*) dtohl(pkg->typeStrings), (void*) pkgSize);
        return BAD_TYPE;
    }
    if ((dtohl(pkg->typeStrings)&0x3) != 0) {
        QUP_LOGI("[-] resTable_package type strings at %p is not on an integer boundary.",
                (void*) dtohl(pkg->typeStrings));
        return BAD_TYPE;
    }
    if (dtohl(pkg->keyStrings) >= pkgSize) {
        QUP_LOGI("[-] resTable_package key strings at %p are past chunk size %p.",
                (void*) dtohl(pkg->keyStrings), (void*) pkgSize);
        return BAD_TYPE;
    }
    if ((dtohl(pkg->keyStrings)&0x3) != 0) {
        QUP_LOGI("[-] resTable_package key strings at %p is not on an integer boundary.",
                (void*) dtohl(pkg->keyStrings));
        return BAD_TYPE;
    }

    Package* package = NULL;
    PackageGroup* group = NULL;
    uint32_t id = idmap_id != 0 ? idmap_id : dtohl(pkg->id);
    // If at this point id == 0, pkg is an overlay package without a
    // corresponding idmap. During regular usage, overlay packages are
    // always loaded alongside their idmaps, but during idmap creation
    // the package is temporarily loaded by itself.
    if (id < 256) {

        package = new Package(header, pkg);
        if (package == NULL) {
            QUP_LOGI("[-] alloc package fail");
            return NO_MEMORY;
        }

        size_t idx = mPackageMap[id];
        if (idx == 0) {
            idx = mPackageGroups.size() + 1;

            uint16_t tmpName[sizeof (pkg->name) / sizeof (uint16_t)];
            strcpy16_dtoh(tmpName, pkg->name, sizeof (pkg->name) / sizeof (uint16_t));
            group = new PackageGroup(String16(tmpName), id);
            if (group == NULL) {
                delete package;
                return NO_MEMORY;
            }

            err = package->typeStrings.setTo(base + dtohl(pkg->typeStrings),
                    header->dataEnd - (base + dtohl(pkg->typeStrings)));
            if (err != NO_ERROR) {
                delete group;
                delete package;
                return err;
            }
            err = package->keyStrings.setTo(base + dtohl(pkg->keyStrings),
                    header->dataEnd - (base + dtohl(pkg->keyStrings)));
            if (err != NO_ERROR) {
                delete group;
                delete package;
                return err;
            }

            //printf("Adding new package id %d at index %d\n", id, idx);
            mPackageGroups.push_back(group);

            group->basePackage = package;

            mPackageMap[id] = (uint8_t) idx;
        } else {
            group = mPackageGroups[idx - 1];
            if (group == NULL) {
                return UNKNOWN_ERROR;
            }
        }
        group->packages.push_back(package);
    } else {
        QUP_LOGI("[-] Package id out of range");
        return NO_ERROR;
    }


    // Iterate through all chunks.
    size_t curPackage = 0;

    const ResChunk_header* chunk =
            (const ResChunk_header*) (((const uint8_t*) pkg)
            + dtohs(pkg->header.headerSize));
    const uint8_t* endPos = ((const uint8_t*) pkg) + dtohs(pkg->header.size);
    while (((const uint8_t*) chunk) <= (endPos - sizeof (ResChunk_header)) &&
            ((const uint8_t*) chunk) <= (endPos - dtohl(chunk->size))) {
        QUP_LOGI("[*] PackageChunk: type=0x%x, headerSize=0x%x, size=0x%x, pos=%p\n",
                dtohs(chunk->type), dtohs(chunk->headerSize), dtohl(chunk->size),
                (void*) (((const uint8_t*) chunk) - ((const uint8_t*) header->header)));
        const size_t csize = dtohl(chunk->size);
        const uint16_t ctype = dtohs(chunk->type);
        if (ctype == RES_TABLE_TYPE_SPEC_TYPE) {
            const ResTable_typeSpec* typeSpec = (const ResTable_typeSpec*) (chunk);
            err = validate_chunk(&typeSpec->header, sizeof (*typeSpec),
                    endPos, "ResTable_typeSpec");
            if (err != NO_ERROR) {
                return err;
            }

            const size_t typeSpecSize = dtohl(typeSpec->header.size);

            QUP_LOGI("[*] TypeSpec off %p: type=0x%x, headerSize=0x%x, size=%p\n",
                    (void*) (base - (const uint8_t*) chunk),
                    dtohs(typeSpec->header.type),
                    dtohs(typeSpec->header.headerSize),
                    (void*) typeSpecSize);
            // look for block overrun or int overflow when multiplying by 4
            if ((dtohl(typeSpec->entryCount) > (INT32_MAX / sizeof (uint32_t))
                    || dtohs(typeSpec->header.headerSize)+(sizeof (uint32_t) * dtohl(typeSpec->entryCount))
                    > typeSpecSize)) {
                QUP_LOGI("[-] ResTable_typeSpec entry index to %p extends beyond chunk end %p.",
                        (void*) (dtohs(typeSpec->header.headerSize)
                        +(sizeof (uint32_t) * dtohl(typeSpec->entryCount))),
                        (void*) typeSpecSize);
                return BAD_TYPE;
            }

            if (typeSpec->id == 0) {
                QUP_LOGI("[-] ResTable_type has an id of 0.");
                return BAD_TYPE;
            }

            while (package->types.size() < typeSpec->id) {
                package->types.push_back(NULL);
            }
            Type* t = package->types[typeSpec->id - 1];
            if (t == NULL) {
                t = new Type(header, package, dtohl(typeSpec->entryCount));
                package->types[(typeSpec->id - 1)] = t;
            } else if (dtohl(typeSpec->entryCount) != t->entryCount) {
                QUP_LOGI("[-] ResTable_typeSpec entry count inconsistent: given %d, previously %d",
                        (int) dtohl(typeSpec->entryCount), (int) t->entryCount);
                return BAD_TYPE;
            }
            t->typeSpecFlags = (const uint32_t*) (
                    ((const uint8_t*) typeSpec) + dtohs(typeSpec->header.headerSize));
            t->typeSpec = typeSpec;

        } else if (ctype == RES_TABLE_TYPE_TYPE) {
            const ResTable_type* type = (const ResTable_type*) (chunk);
            err = validate_chunk(&type->header, sizeof (*type) - sizeof (ResTable_config) + 4,
                    endPos, "ResTable_type");
            if (err != NO_ERROR) {
                return err;
            }

            const size_t typeSize = dtohl(type->header.size);

            QUP_LOGI("[*] Type off %p: type=0x%x, headerSize=0x%x, size=%p\n",
                    (void*) (base - (const uint8_t*) chunk),
                    dtohs(type->header.type),
                    dtohs(type->header.headerSize),
                    (void*) typeSize);
            if (dtohs(type->header.headerSize)+(sizeof (uint32_t) * dtohl(type->entryCount))
                    > typeSize) {
                QUP_LOGI("[-] ResTable_type entry index to %p extends beyond chunk end %p.",
                        (void*) (dtohs(type->header.headerSize)
                        +(sizeof (uint32_t) * dtohl(type->entryCount))),
                        (void*) typeSize);
                return BAD_TYPE;
            }
            if (dtohl(type->entryCount) != 0
                    && dtohl(type->entriesStart) > (typeSize - sizeof (ResTable_entry))) {
                QUP_LOGI("[-] ResTable_type entriesStart at %p extends beyond chunk end %p.",
                        (void*) dtohl(type->entriesStart), (void*) typeSize);
                return BAD_TYPE;
            }
            if (type->id == 0) {
                QUP_LOGI("[-] ResTable_type has an id of 0.");
                return BAD_TYPE;
            }

            while (package->types.size() < type->id) {
                package->types.push_back(NULL);
            }
            Type* t = package->types[type->id - 1];
            if (t == NULL) {
                t = new Type(header, package, dtohl(type->entryCount));
                package->types[(type->id - 1)] = t;
            } else if (dtohl(type->entryCount) != t->entryCount) {
                QUP_LOGI("ResTable_type entry count inconsistent: given %d, previously %d",
                        (int) dtohl(type->entryCount), (int) t->entryCount);
                return BAD_TYPE;
            }

            TABLE_GETENTRY(
                    ResTable_config thisConfig;
                    thisConfig.copyFromDtoH(type->config);
                    ALOGI("Adding config to type %d: %s\n",
                    type->id, thisConfig.toString().string()));
            t->configs.push_back(type);
        } else {
            status_t err = validate_chunk(chunk, sizeof (ResChunk_header),
                    endPos, "ResTable_package:unknown");
            if (err != NO_ERROR) {
                return err;
            }
        }
        chunk = (const ResChunk_header*)
                (((const uint8_t*) chunk) + csize);
    }

    if (group->typeCount == 0) {
        group->typeCount = package->types.size();
    }

    return NO_ERROR;
}

