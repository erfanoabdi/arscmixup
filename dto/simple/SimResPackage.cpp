/* 
 * File:   SimResPackage.cpp
 * Author : angelToms
 * 
 */

#include "SimResPackage.h"

#include "SimResTypeSpec.h"
#include "SimResType.h"
#include "../../QupLog.h"
#include "../../ByteOrder.h"
#include "../../ResIncludes.h"
#include "../../utils/String8.h"
#include "../../utils/String16.h"
#include "../ResTabHeader.h"

SimResPackage::SimResPackage(u1* data, bool isCopy, struct ResTable_header* header,
        SimResStringPool* stringPool) :
mData(data), mSize(0), mIsCopy(isCopy), mHeader(header), mResPackage(NULL),
mStringPool(stringPool), mTypeStrings(NULL), mKeyStrings(NULL) {

}

SimResPackage::~SimResPackage() {
    if (mTypeStrings) {
        delete mTypeStrings;
        mTypeStrings = NULL;
    }
    if (mKeyStrings) {
        delete mKeyStrings;
        mKeyStrings = NULL;
    }

    int size = mResTypes.size();
    for (int i = 0; i < size; i++) {
        SimResTypes* types = mResTypes[i];
        delete types;
        types = NULL;
    }
    mResTypes.clear();
}

bool SimResPackage::makeResPackage() {
    u1* p = mData;
    u1* dataEnd = mData + mHeader->header.size;

    mResPackage = (ResTable_package*) p;
    uint16_t tmpName[sizeof (mResPackage->name) / sizeof (uint16_t)];
    strcpy16_dtoh(tmpName, mResPackage->name, sizeof (mResPackage->name) / sizeof (uint16_t));
    const char* name = String8(String16(tmpName)).string();
    mPkgName = std::string(name);
    printResTablePackage(mResPackage, name);

    status_t err = validate_chunk(&mResPackage->header, sizeof (*mResPackage),
            dataEnd, "ResTable_package");
    if (err != NO_ERROR) {
        return false;
    }
    const size_t pkgSize = dtohl(mResPackage->header.size);
    QUP_LOGI("[*] res pkg size = %u", pkgSize);
    mSize = pkgSize;

    mTypeStrings = new SimResStringPool((p + dtohl(mResPackage->typeStrings)), mIsCopy);
    if (!mTypeStrings) {
        QUP_LOGI("[-] alloc pkg type string fail");
        return false;
    }

    if (!mTypeStrings->makeResStringPool()) {
        return false;
    }

    mKeyStrings = new SimResStringPool((p + dtohl(mResPackage->keyStrings)), mIsCopy);
    if (!mKeyStrings) {
        QUP_LOGI("[-] alloc pkg key string fail");
        return false;
    }

    if (!mKeyStrings->makeResStringPool()) {
        return false;
    }

    if (!makeTypes()) {
        return false;
    }

    return true;
}

bool SimResPackage::makeResTypeSpec(const u1* chunk, const u1* endPos) {
    const ResTable_typeSpec* typeSpec = (const ResTable_typeSpec*) (chunk);

    status_t err = validate_chunk(&typeSpec->header, sizeof (*typeSpec),
            endPos, "ResTable_typeSpec");
    if (err != NO_ERROR) {
        return false;
    }

    printResTabTypeSepc(typeSpec);
    while (mResTypes.size() < typeSpec->id) {
        mResTypes.push_back(NULL);
    }

    SimResTypes* t = mResTypes[typeSpec->id - 1];
    SimResTypeSpec* tc = NULL;
    if (t == NULL) {
        tc = new SimResTypeSpec(dtohl(typeSpec->entryCount), typeSpec);
        if (!tc) {
            QUP_LOGI("[-] calloc SimResTypeSpec fail, not enought memrory");
            return false;
        }

        t = new SimResTypes(typeSpec->id,
                dtohl(typeSpec->entryCount));
        if (!t) {
            QUP_LOGI("[-] calloc SimResTypes fail, not enought memrory");
            return false;
        }

        mResTypes[(typeSpec->id - 1)] = t;
        t->mTypeSpec = tc;
    } else if (dtohl(typeSpec->entryCount) != t->mEntryCount) {
        QUP_LOGI("[-] ResTable_typeSpec entry count inconsistent: given %d, previously %d",
                (int) dtohl(typeSpec->entryCount), (int) t->mEntryCount);
        return false;
    }
    tc->mTypeSpecEntries = (const uint32_t*) (
            ((const uint8_t*) typeSpec) + dtohs(typeSpec->header.headerSize));
    return true;
}

bool SimResPackage::makeResType(const u1* chunk, const u1* endPos) {
    const ResTable_type* type = (const ResTable_type*) (chunk);
    status_t err = validate_chunk(&type->header, sizeof (*type) - sizeof (ResTable_config) + 4,
            endPos, "ResTable_type");
    if (err != NO_ERROR) {
        return false;
    }

    printResTabType(type);

    while (mResTypes.size() < type->id) {
        mResTypes.push_back(NULL);
    }

    SimResTypes* t = mResTypes[type->id - 1];

    if (t == NULL) {
        t = new SimResTypes(type->id, dtohl(type->entryCount));
        if (!t) {
            QUP_LOGI("[-] calloc SimResTypes fail, not enought memrory");
            return false;
        }
        mResTypes[type->id - 1] = t;
    }

    TABLE_GETENTRY(
            ResTable_config thisConfig;
            copyFromDtoH(type->config, thisConfig);
            QUP_LOGI("Adding config to type %d: %s\n",
            type->id, toString(thisConfig).string()));

    SimResType* resType = new SimResType(type, dtohl(type->entryCount));
    if (!resType) {
        QUP_LOGI("[-] calloc sim res type fail, not enought memory");
        return false;
    }
    resType->mEntries = (const uint32_t*) (
            ((const uint8_t*) type) + dtohs(type->header.headerSize));

    t->add(resType);

    return true;
}

bool SimResPackage::makeTypes() {
    status_t err = NO_ERROR;
    const ResChunk_header* chunk =
            (const ResChunk_header*) (((const uint8_t*) mResPackage)
            + dtohs(mResPackage->header.headerSize));
    const uint8_t* endPos = ((const uint8_t*) mResPackage) + dtohs(mResPackage->header.size);

    while (((const uint8_t*) chunk) <= (endPos - sizeof (ResChunk_header)) &&
            ((const uint8_t*) chunk) <= (endPos - dtohl(chunk->size))) {

        QUP_LOGI("[*] PackageChunk: type=0x%x, headerSize=0x%x, size=0x%x, pos=%p",
                dtohs(chunk->type), dtohs(chunk->headerSize), dtohl(chunk->size),
                (void*) (((const uint8_t*) chunk) - ((const uint8_t*) mHeader)));

        const size_t csize = dtohl(chunk->size);
        const uint16_t ctype = dtohs(chunk->type);

        if (ctype == RES_TABLE_TYPE_SPEC_TYPE) {
            if (!makeResTypeSpec((u1*) chunk, endPos))
                return false;
        } else if (ctype == RES_TABLE_TYPE_TYPE) {
            if (!makeResType((u1*) chunk, endPos))
                return false;
        } else {
            status_t err = validate_chunk(chunk, sizeof (ResChunk_header),
                    endPos, "ResTable_package:unknown");
            if (err != NO_ERROR) {
                return false;
            }
        }
        chunk = (const ResChunk_header*)
                (((const uint8_t*) chunk) + csize);
    }

    return true;
}

size_t SimResPackage::getPkgSize() {
    return mSize;
}

void SimResPackage::printValue(const Res_value& value) {
    if (value.dataType == Res_value::TYPE_NULL) {
        QUP_LOGI("[*] (null)\n");
    } else if (value.dataType == Res_value::TYPE_REFERENCE) {
        QUP_LOGI("[*] (reference) 0x%08x\n", value.data);
    } else if (value.dataType == Res_value::TYPE_ATTRIBUTE) {
        QUP_LOGI("[*] (attribute) 0x%08x\n", value.data);
    } else if (value.dataType == Res_value::TYPE_STRING) {
        const u1* str8 = mStringPool->getStringByEntryIndex(value.data);
        if (str8 != NULL) {
            QUP_LOGI("[*] (string value) \"%s\"\n", str8);
        }
    } else if (value.dataType == Res_value::TYPE_FLOAT) {
        QUP_LOGI("[*] (float) %g\n", *(const float*) &value.data);
    } else if (value.dataType == Res_value::TYPE_DIMENSION) {
        QUP_LOGI("[*] (dimension) ");
        print_complex(value.data, false);
        QUP_LOGI("[*] \n");
    } else if (value.dataType == Res_value::TYPE_FRACTION) {
        QUP_LOGI("[*] (fraction) ");
        print_complex(value.data, true);
        QUP_LOGI("[*] \n");
    } else if (value.dataType >= Res_value::TYPE_FIRST_COLOR_INT
            || value.dataType <= Res_value::TYPE_LAST_COLOR_INT) {
        QUP_LOGI("[*] (color) #%08x\n", value.data);
    } else if (value.dataType == Res_value::TYPE_INT_BOOLEAN) {
        QUP_LOGI("[*] (boolean) %s\n", value.data ? "true" : "false");
    } else if (value.dataType >= Res_value::TYPE_FIRST_INT
            || value.dataType <= Res_value::TYPE_LAST_INT) {
        QUP_LOGI("[*] (int) 0x%08x or %d\n", value.data, value.data);
    } else {
        QUP_LOGI("[*] (unknown type) t=0x%02x d=0x%08x (s=0x%04x r=0x%02x)\n",
                (int) value.dataType, (int) value.data,
                (int) value.size, (int) value.res0);
    }
}

void SimResPackage::printEntryMap(const ResTable_entry* ent, size_t esize,
        int typeSize, const struct ResTable_map_entry* bagPtr) {
    Res_value value;
    const int N = dtohl(bagPtr->count);
    const uint8_t* baseMapPtr = (const uint8_t*) ent;
    size_t mapOffset = esize;
    const ResTable_map* mapPtr = (ResTable_map*) (baseMapPtr + mapOffset);
    QUP_LOGI("[*] Parent=0x%08x, Count=%d\n",
            dtohl(bagPtr->parent.ident), N);
    for (int i = 0; i < N && mapOffset < (typeSize - sizeof (ResTable_map)); i++) {
        QUP_LOGI("[*] #%i (Key=0x%08x): ",
                i, dtohl(mapPtr->name.ident));
        copyFrom_dtoh(mapPtr->value, value);
        printValue(value);
        const size_t size = dtohs(mapPtr->value.size);
        mapOffset += size + sizeof (*mapPtr) - sizeof (mapPtr->value);
        mapPtr = (ResTable_map*) (baseMapPtr + mapOffset);
    }
}

void SimResPackage::printPkgString(bool isAll) {

    if (isAll) {
        mTypeStrings->printStringPool();
        mKeyStrings->printStringPool();
    }

    int size = mResTypes.size();
    for (int i = 0; i < size; i++) {
        SimResTypes* types = mResTypes[i];

        QUP_LOGI("\n");
        QUP_LOGI("[*] print res type spec ...");
        types->mTypeSpec->printTypeSpecConfig();

        QUP_LOGI("\n");
        QUP_LOGI("[*] print res type ...");
        for (int j = 0; j < types->mTypes.size(); j++) {
            uint32_t resID = (0xff000000 & ((mResPackage->id) << 24))
                    | (0x00ff0000 & ((i + 1) << 16))
                    | (0x0000ffff & (j));

            QUP_LOGI("[*] resID = 0x%.8x", resID);

            SimResType* stype = types->mTypes[j];
            const ResTable_type* type = stype->mType;

            if ((((uint64_t) type)&0x3) != 0) {
                QUP_LOGI("[+] NON-INTEGER ResTable_type ADDRESS: %p\n", type);
                continue;
            }

            String8 configStr = configToString(type->config);
            QUP_LOGI("[+] Rec_config %s:\n", configStr.size() > 0
                    ? configStr.string() : "(default)");

            u4 entriesStart = dtohl(type->entriesStart);

            if ((entriesStart & 0x3) != 0) {
                QUP_LOGI("[+] NON-INTEGER ResTable_type entriesStart OFFSET: %p\n", (void*) entriesStart);
                continue;
            }

            u4 typeSize = dtohl(type->header.size);
            if ((typeSize & 0x3) != 0) {
                QUP_LOGI("[+] NON-INTEGER ResTable_type header.size: %p\n", (void*) typeSize);
                continue;
            }

            for (int k = 0; k < stype->mEntryCount; k++) {
                const u4 off = dtohl(stype->mEntries[k]);
                //empty entry
                if (off == ResTable_type::NO_ENTRY)
                    continue;

                resID = (0xff000000 & ((mResPackage->id) << 24))
                        | (0x00ff0000 & ((i + 1) << 16))
                        | (0x0000ffff & (k));

                QUP_LOGI("[*] type resID = 0x%.8x", resID);

                if (((entriesStart + off)&0x3) != 0) {
                    QUP_LOGI("[*] [-] NON-INTEGER ResTable_entry OFFSET: %p\n",
                            (void*) (entriesStart + off));
                    continue;
                }

                QUP_LOGI("[*] ResTable_type[%d] entries[%d] = 0x%08x",
                        type->id, k, off);

                const ResTable_entry* ent = (const ResTable_entry*) ((u1*) type + entriesStart + off);
                printResTableEntry(ent);
                const u1* rname = mKeyStrings->getStringByEntryIndex(ent->key.index);
                if (rname != NULL) {
                    QUP_LOGI("[+] entry name = %s:%s/%s", mPkgName.c_str(),
                            mTypeStrings->getStringByEntryIndex(i), rname);
                } else {
                    QUP_LOGI("\n");
                }

                if (ent->flags & ResTable_entry::FLAG_COMPLEX) {
                    const struct ResTable_map_entry* mapEntry = (const ResTable_map_entry*) ent;
                    printResTableMapEntry(mapEntry);

                    printEntryMap(ent, ent->size, typeSize, mapEntry);
                } else {
                    const struct Res_value* val = (const Res_value*)
                            (((const u1*) ent) + dtohs(ent->size));
                    printValue(*val);
                }
            }
        }
    }
}

