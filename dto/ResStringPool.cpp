/* 
 * File:   ResStringPool.cpp
 * Author : angelToms
 * 
 */

#include "ResStringPool.h"
#include "../utils/Unicode.h"

ResStringPool::ResStringPool()
: mError(NO_INIT), mOwnedData(NULL), mHeader(NULL), mCache(NULL) {
}

ResStringPool::ResStringPool(const void* data, size_t size, bool copyData)
: mError(NO_INIT), mOwnedData(NULL), mHeader(NULL), mCache(NULL) {
    setTo(data, size, copyData);
}

ResStringPool::~ResStringPool() {
    uninit();
}

status_t ResStringPool::getError() const {
    return mError;
}

void ResStringPool::uninit() {
    mError = NO_INIT;
    if (mHeader != NULL && mCache != NULL) {
        for (size_t x = 0; x < mHeader->stringCount; x++) {
            if (mCache[x] != NULL) {
                free(mCache[x]);
                mCache[x] = NULL;
            }
        }
        free(mCache);
        mCache = NULL;
    }
    if (mOwnedData) {
        free(mOwnedData);
        mOwnedData = NULL;
    }
}

status_t ResStringPool::setTo(const void* data, size_t size, bool copyData) {
    if (!data || !size) {
        return (mError = BAD_TYPE);
    }

    uninit();

    const bool notDeviceEndian = htods(0xf0) != 0xf0;

    QUP_LOGI("[*] notDeviceEndian = %d", notDeviceEndian);
    if (copyData || notDeviceEndian) {
        mOwnedData = malloc(size);
        if (mOwnedData == NULL) {
            return (mError = NO_MEMORY);
        }
        memcpy(mOwnedData, data, size);
        data = mOwnedData;
    }

    mHeader = (const ResStringPool_header*) data;

    printResStringPoolHeader(mHeader);

    if (notDeviceEndian) {
        ResStringPool_header* h = const_cast<ResStringPool_header*> (mHeader);
        h->header.headerSize = dtohs(mHeader->header.headerSize);
        h->header.type = dtohs(mHeader->header.type);
        h->header.size = dtohl(mHeader->header.size);
        h->stringCount = dtohl(mHeader->stringCount);
        h->styleCount = dtohl(mHeader->styleCount);
        h->flags = dtohl(mHeader->flags);
        h->stringsStart = dtohl(mHeader->stringsStart);
        h->stylesStart = dtohl(mHeader->stylesStart);
    }

    if (mHeader->header.headerSize > mHeader->header.size
            || mHeader->header.size > size) {
        QUP_LOGI("[-] bad string block: header size %d or total size %d is larger than data size %d\n",
                (int) mHeader->header.headerSize, (int) mHeader->header.size, (int) size);
        return (mError = BAD_TYPE);
    }
    mSize = mHeader->header.size;
    mEntries = (const uint32_t*)
            (((const uint8_t*) data) + mHeader->header.headerSize);

    if (mHeader->stringCount > 0) {
        if ((mHeader->stringCount * sizeof (uint32_t) < mHeader->stringCount) // uint32 overflow?
                || (mHeader->header.headerSize + (mHeader->stringCount * sizeof (uint32_t)))
                > size) {
            QUP_LOGI("[-] bad string block: entry of %d items extends past data size %d\n",
                    (int) (mHeader->header.headerSize + (mHeader->stringCount * sizeof (uint32_t))),
                    (int) size);
            return (mError = BAD_TYPE);
        }

        size_t charSize;
        if (mHeader->flags & UTF8_FLAG) {
            charSize = sizeof (uint8_t);
        } else {
            charSize = sizeof (uint16_t);
        }

        QUP_LOGI("[*] char size = %u", charSize);

        //里面引用的字符串表的起始地址， 如res/drawable/above_shadow.xml
        mStrings = (const void*)
                (((const uint8_t*) data) + mHeader->stringsStart);
        if (mHeader->stringsStart >= (mHeader->header.size - sizeof (uint16_t))) {
            QUP_LOGI("[-] bad string block: string pool starts at %d, after total size %d\n",
                    (int) mHeader->stringsStart, (int) mHeader->header.size);
            return (mError = BAD_TYPE);
        }
        if (mHeader->styleCount == 0) {
            mStringPoolSize =
                    (mHeader->header.size - mHeader->stringsStart) / charSize;
        } else {
            // check invariant: styles starts before end of data
            if (mHeader->stylesStart >= (mHeader->header.size - sizeof (uint16_t))) {
                QUP_LOGI("[-] bad style block: style block starts at %d past data size of %d\n",
                        (int) mHeader->stylesStart, (int) mHeader->header.size);
                return (mError = BAD_TYPE);
            }
            // check invariant: styles follow the strings
            if (mHeader->stylesStart <= mHeader->stringsStart) {
                QUP_LOGI("[-] bad style block: style block starts at %d, before strings at %d\n",
                        (int) mHeader->stylesStart, (int) mHeader->stringsStart);
                return (mError = BAD_TYPE);
            }
            mStringPoolSize =
                    (mHeader->stylesStart - mHeader->stringsStart) / charSize;
        }

        QUP_LOGI("[*] string pool size = %u", mStringPoolSize);

        // check invariant: stringCount > 0 requires a string pool to exist
        if (mStringPoolSize == 0) {
            QUP_LOGI("[-] bad string block: stringCount is %d but pool size is 0\n", (int) mHeader->stringCount);
            return (mError = BAD_TYPE);
        }

        if (notDeviceEndian) {
            size_t i;
            uint32_t* e = const_cast<uint32_t*> (mEntries);
            for (i = 0; i < mHeader->stringCount; i++) {
                e[i] = dtohl(mEntries[i]);
            }
            if (!(mHeader->flags & UTF8_FLAG)) {
                const uint16_t* strings = (const uint16_t*) mStrings;
                uint16_t* s = const_cast<uint16_t*> (strings);
                for (i = 0; i < mStringPoolSize; i++) {
                    s[i] = dtohs(strings[i]);
                }
            }
        }

        if ((mHeader->flags & UTF8_FLAG &&
                ((uint8_t*) mStrings)[mStringPoolSize - 1] != 0) ||
                (!mHeader->flags & UTF8_FLAG &&
                ((uint16_t*) mStrings)[mStringPoolSize - 1] != 0)) {
            QUP_LOGI("[-] bad string block: last string is not 0-terminated\n");
            return (mError = BAD_TYPE);
        }
    } else {
        mStrings = NULL;
        mStringPoolSize = 0;
    }

    if (mHeader->styleCount > 0) {
        mEntryStyles = mEntries + mHeader->stringCount;
        // invariant: integer overflow in calculating mEntryStyles
        if (mEntryStyles < mEntries) {
            QUP_LOGI("[-] bad string block: integer overflow finding styles\n");
            return (mError = BAD_TYPE);
        }

        if (((const uint8_t*) mEntryStyles - (const uint8_t*) mHeader) > (int) size) {
            QUP_LOGI("[-] bad string block: entry of %d styles extends past data size %d\n",
                    (int) ((const uint8_t*) mEntryStyles - (const uint8_t*) mHeader),
                    (int) size);
            return (mError = BAD_TYPE);
        }
        mStyles = (const uint32_t*)
                (((const uint8_t*) data) + mHeader->stylesStart);
        if (mHeader->stylesStart >= mHeader->header.size) {
            QUP_LOGI("[-] bad string block: style pool starts %d, after total size %d\n",
                    (int) mHeader->stylesStart, (int) mHeader->header.size);
            return (mError = BAD_TYPE);
        }
        mStylePoolSize =
                (mHeader->header.size - mHeader->stylesStart) / sizeof (uint32_t);

        if (notDeviceEndian) {
            size_t i;
            uint32_t* e = const_cast<uint32_t*> (mEntryStyles);
            for (i = 0; i < mHeader->styleCount; i++) {
                e[i] = dtohl(mEntryStyles[i]);
            }
            uint32_t* s = const_cast<uint32_t*> (mStyles);
            for (i = 0; i < mStylePoolSize; i++) {
                s[i] = dtohl(mStyles[i]);
            }
        }

        const ResStringPool_span endSpan = {
            { htodl(ResStringPool_span::END)},
            htodl(ResStringPool_span::END), htodl(ResStringPool_span::END)
        };

        if (memcmp(&mStyles[mStylePoolSize - (sizeof (endSpan) / sizeof (uint32_t))],
                &endSpan, sizeof (endSpan)) != 0) {
            QUP_LOGI("[-] bad string block: last style is not 0xFFFFFFFF-terminated\n");
            return (mError = BAD_TYPE);
        }
    } else {
        mEntryStyles = NULL;
        mStyles = NULL;
        mStylePoolSize = 0;
    }

    return (mError = NO_ERROR);
}

const uint16_t* ResStringPool::stringAt(size_t idx, size_t* u16len) const
{
    if (mError == NO_ERROR && idx < mHeader->stringCount) {
        const bool isUTF8 = (mHeader->flags&UTF8_FLAG) != 0;
        const uint32_t off = mEntries[idx]/(isUTF8?sizeof(char):sizeof(uint16_t));
        if (off < (mStringPoolSize-1)) {
            if (!isUTF8) {
                const uint16_t* strings = (uint16_t*)mStrings;
                const uint16_t* str = strings+off;

                *u16len = decodeLength16(&str);
                if ((uint32_t)(str+*u16len-strings) < mStringPoolSize) {
                    return str;
                } else {
                    QUP_LOGI("[-] Bad string block: string #%d extends to %d, past end at %d\n",
                            (int)idx, (int)(str+*u16len-strings), (int)mStringPoolSize);
                }
            } else {
                const uint8_t* strings = (uint8_t*)mStrings;
                const uint8_t* u8str = strings+off;

                *u16len = decodeLength8(&u8str);
                size_t u8len = decodeLength8(&u8str);

                // encLen must be less than 0x7FFF due to encoding.
                if ((uint32_t)(u8str+u8len-strings) < mStringPoolSize) {

                    if (mCache == NULL) {
#ifndef HAVE_ANDROID_OS
                        STRING_POOL_NOISY(ALOGI("CREATING STRING CACHE OF %d bytes",
                                mHeader->stringCount*sizeof(uint16_t**)));
#else
                        // We do not want to be in this case when actually running Android.
                        ALOGW("CREATING STRING CACHE OF %d bytes",
                                mHeader->stringCount*sizeof(uint16_t**));
#endif
                        mCache = (uint16_t**)calloc(mHeader->stringCount, sizeof(uint16_t**));
                        if (mCache == NULL) {
                            QUP_LOGI("[-] No memory trying to allocate decode cache table of %d bytes\n",
                                    (int)(mHeader->stringCount*sizeof(uint16_t**)));
                            return NULL;
                        }
                    }

                    if (mCache[idx] != NULL) {
                        return mCache[idx];
                    }

                    ssize_t actualLen = utf8_to_utf16_length(u8str, u8len);
                    if (actualLen < 0 || (size_t)actualLen != *u16len) {
                        QUP_LOGI("[-] Bad string block: string #%lld decoded length is not correct "
                                "%lld vs %llu\n",
                                (long long)idx, (long long)actualLen, (long long)*u16len);
                        return NULL;
                    }

                    uint16_t *u16str = (uint16_t *)calloc(*u16len+1, sizeof(uint16_t));
                    if (!u16str) {
                        QUP_LOGI("[-] No memory when trying to allocate decode cache for string #%d\n",
                                (int)idx);
                        return NULL;
                    }

                    STRING_POOL_NOISY(ALOGI("Caching UTF8 string: %s", u8str));
                    utf8_to_utf16(u8str, u8len, u16str);
                    mCache[idx] = u16str;
                    return u16str;
                } else {
                    QUP_LOGI("[-] Bad string block: string #%lld extends to %lld, past end at %lld\n",
                            (long long)idx, (long long)(u8str+u8len-strings),
                            (long long)mStringPoolSize);
                }
            }
        } else {
            QUP_LOGI("[-] Bad string block: string #%d entry is at %d, past end at %d\n",
                    (int)idx, (int)(off*sizeof(uint16_t)),
                    (int)(mStringPoolSize*sizeof(uint16_t)));
        }
    }
    return NULL;
}

 const char* ResStringPool::string8At(size_t idx, size_t* outLen) const {
    if (mError == NO_ERROR && idx < mHeader->stringCount) {
        if ((mHeader->flags&UTF8_FLAG) == 0) {
            return NULL;
        }
        const uint32_t off = mEntries[idx]/sizeof(char);
        if (off < (mStringPoolSize-1)) {
            const uint8_t* strings = (uint8_t*)mStrings;
            const uint8_t* str = strings+off;
            *outLen = decodeLength8(&str);
            size_t encLen = decodeLength8(&str);
            if ((uint32_t)(str+encLen-strings) < mStringPoolSize) {
                return (const char*)str;
            } else {
                QUP_LOGI("[-] Bad string block: string #%d extends to %d, past end at %d\n",
                        (int)idx, (int)(str+encLen-strings), (int)mStringPoolSize);
            }
        } else {
            QUP_LOGI("[-] Bad string block: string #%d entry is at %d, past end at %d\n",
                    (int)idx, (int)(off*sizeof(uint16_t)),
                    (int)(mStringPoolSize*sizeof(uint16_t)));
        }
    }
    return NULL;
}




