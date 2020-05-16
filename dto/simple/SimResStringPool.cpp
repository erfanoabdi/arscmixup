/* 
 * File:   SimResStringPool.cpp
 * Author : angelToms
 * 
 */

#include "SimResStringPool.h"
#include "../../QupLog.h"
#include "../../ByteOrder.h"
#include "../../ResIncludes.h"
#include "../../utils/String8.h"
#include "../../utils/String16.h"
#include "../ResTabHeader.h"

SimResStringPool::SimResStringPool(u1* data, bool isCopy) : mData(data),
mIsCopy(isCopy), mSize(0), mStringPoolHeader(NULL),
mStrings(NULL), mStyles(NULL), mStringEntries(NULL), mStyleEntries(NULL),
mStringCount(0), mStyleCount(0), mStringPoolSize(0), mStylePoolSize(0) {
    //ResStringPool
    mData = data;
    mStringPoolHeader = (struct ResStringPool_header*) data;
    mSize = mStringPoolHeader->header.size;
    if (isCopy) {
        mownData = new u1[mSize];
        if (!mownData) {
            QUP_LOGI("[-] alloc res string own data fail ,not enougnt memory!");
            exit(-1);
        }
        memcpy(mownData, data, mSize);
    }
}

SimResStringPool::~SimResStringPool() {
    if (mIsCopy) {
        delete mownData;
        mownData = NULL;
    }
}

bool SimResStringPool::makeResStringPool() {

    printResStringPoolHeader(mStringPoolHeader);
    const bool notDeviceEndian = htods(0xf0) != 0xf0;
    QUP_LOGI("[*] notDeviceEndian = %d", notDeviceEndian);


    mStringEntries = (u4*)
            (((const uint8_t*) mStringPoolHeader) + mStringPoolHeader->header.headerSize);

    size_t charSize;
    if (mStringPoolHeader->flags & UTF8_FLAG) {
        charSize = sizeof (uint8_t);
    } else {
        charSize = sizeof (uint16_t);
    }

    QUP_LOGI("[*] char size = %u", charSize);

    mStringCount = mStringPoolHeader->stringCount;
    mStyleCount = mStringPoolHeader->styleCount;

    mStrings = (u1*)
            (((const uint8_t*) mStringPoolHeader) + mStringPoolHeader->stringsStart);

    if (mStringPoolHeader->styleCount == 0) {
        mStringPoolSize =
                (mStringPoolHeader->header.size - mStringPoolHeader->stringsStart) / charSize;
    } else {
        mStringPoolSize =
                (mStringPoolHeader->stylesStart - mStringPoolHeader->stringsStart) / charSize;
    }

    if ((mStringPoolHeader->flags & UTF8_FLAG &&
            ((uint8_t*) mStrings)[mStringPoolSize - 1] != 0) ||
            (!mStringPoolHeader->flags & UTF8_FLAG &&
            ((uint16_t*) mStrings)[mStringPoolSize - 1] != 0)) {
        QUP_LOGI("[-] bad string block: last string is not 0-terminated\n");
        return false;
    }

    if (mStringPoolHeader->styleCount > 0) {
        mStyleEntries = mStringEntries + mStringPoolHeader->stringCount;

        mStyles = (u1*)
                (((const u1*) mStringPoolHeader) + mStringPoolHeader->stylesStart);

        mStylePoolSize =
                (mStringPoolHeader->header.size - mStringPoolHeader->stylesStart) / sizeof (uint32_t);

        const ResStringPool_span endSpan = {
            { htodl(ResStringPool_span::END)},
            htodl(ResStringPool_span::END), htodl(ResStringPool_span::END)
        };

        if (memcmp(&((u4*) mStyles)[mStylePoolSize - (sizeof (endSpan) / sizeof (uint32_t))],
                &endSpan, sizeof (endSpan)) != 0) {
            QUP_LOGI("[-] bad string block: last style is not 0xFFFFFFFF-terminated\n");
            return false;
        }
    }

    QUP_LOGI("[*] string pool size = %u, style pool size = %u\n", mStringPoolSize,
            mStylePoolSize);
    return true;
}

const u1* SimResStringPool::getStringByEntryIndex(u4 index) {
    if (index < mStringCount) {
        const bool isUTF8 = (mStringPoolHeader->flags & UTF8_FLAG) != 0;
        const u4 off = mStringEntries[index] / (isUTF8 ? sizeof (char) : sizeof (uint16_t));
        QUP_LOGI("[*] is utf8 = %d, off = %d", isUTF8, off);
        if (off < (mStringPoolSize - 1)) {
            const u1* strings = (u1*) mStrings;
            const u1* u8str = strings + off + sizeof (u2);
            return u8str;
        }
    }
    return NULL;
}

const u1 * SimResStringPool::getStyleByEntryIndex(u4 index) {
    if (index < mStyleCount) {
        const bool isUTF8 = (mStringPoolHeader->flags & UTF8_FLAG) != 0;
        const u4 off = mStyleEntries[index] / (isUTF8 ? sizeof (char) : sizeof (uint16_t));
        QUP_LOGI("[*] is utf8 = %d, off = %d", isUTF8, off);
        if (off < (mStylePoolSize - 1)) {
            const u1* strings = (u1*) mStyles;
            const u1* u8str = strings + off + sizeof (u2);
            return u8str;
        }
    }
    return NULL;
}

void SimResStringPool::printStringPool() {
    QUP_LOGI("[*] print string pool ...");

    for (int i = 0; i < mStringCount; i++) {
        const u4 off = mStringEntries[i] / sizeof (char);
        const u1* strings = (u1*) mStrings;
        const u1* str8 = strings + off;
        //我选择UTF-8，如果编码不一样，你需要修改
        size_t outLen = decodeLength8(&str8);

        const u1* str = mStrings + mStringEntries[i] + sizeof (u2);
        QUP_LOGI("[*] pool[%d] = %u, string len = %u,"
                " strlen str = %d, string = %s",
                i,
                mStringEntries[i],
                outLen,
                strlen((const char*) str),
                str);
    }

    QUP_LOGI("[*] print style pool ...");
    QUP_LOGI("[*] ResStringPool_span size = %d", sizeof (struct ResStringPool_span));
    for (int i = 0; i < mStyleCount; i++) {
        u1* sty = mStyles + mStyleEntries[i];
        ResStringPool_span* span = (ResStringPool_span*) sty;
        u4 end = htodl(ResStringPool_span::END);
        if (memcmp((u4*) (&span->name), &end, sizeof (u4)) == 0)
            continue;
        QUP_LOGI("[*] res pool span [%d] = %u, name = 0x%.8x, ResStringPool_ref index = %u,"
                "first char index = %d, last char index = %d",
                i, mStyleEntries[i], span->name, span->name.index,
                span->firstChar, span->lastChar);
    }
}

void SimResStringPool::mixupStringPool(std::map<std::string, std::string>& maps) {
    QUP_LOGI("[*] begin mixup string pool ...");

    for (int i = 0; i < mStringCount; i++) {
        const u4 off = mStringEntries[i] / sizeof (char);
        const u1* strings = (u1*) mStrings;
        const u1* str8 = strings + off;
        //我选择UTF-8，如果编码不一样，你需要修改
        size_t outLen = decodeLength8(&str8);

        u1* str = mStrings + mStringEntries[i] + sizeof (u2);
        int slen = strlen((const char*) str);

        std::string key((const char*) str);
        std::map<std::string, std::string>::iterator iter;

        iter = maps.find(key);
        if (iter != maps.end()) {
            //mixup
            memset(str, 0, slen);
            const char* mixVal = (iter->second).c_str();
            memcpy(str, mixVal, strlen(mixVal));
#ifdef DEBUG_ALL
            QUP_LOGI("[*] pool[%d] = %u, string len = %u,"
                    " strlen str = %d, string = %s, mix string = %s",
                    i,
                    mStringEntries[i],
                    outLen,
                    slen,
                    key.c_str(),
                    str);
#endif
        } else {
            continue;
        }
    }
    QUP_LOGI("[*] end  mixup string pool ...");
}