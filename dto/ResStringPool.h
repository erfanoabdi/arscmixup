/* 
 * File:   ResStringPool.h
 * Author : angelToms
 *
 */

#ifndef RESSTRINGPOOL_H
#define	RESSTRINGPOOL_H

#include "../ResIncludes.h"
#include "../ByteOrder.h"

#include <stdint.h>

/**
 * Convenience class for accessing data in a ResStringPool resource.
 */
class ResStringPool
{
public:
    ResStringPool();
    ResStringPool(const void* data, size_t size, bool copyData=false);
    ~ResStringPool();
    
    status_t setTo(const void* data, size_t size, bool copyData=false);
    status_t getError() const;
    void uninit();

    // Return string entry as UTF16; if the pool is UTF8, the string will
    // be converted before returning.
    inline const char16_t* stringAt(const ResStringPool_ref& ref, size_t* outLen) const {
        return stringAt(ref.index, outLen);
    }
    const char16_t* stringAt(size_t idx, size_t* outLen) const;

    // Note: returns null if the string pool is not UTF8.
    const char* string8At(size_t idx, size_t* outLen) const;

    
    // Return string whether the pool is UTF8 or UTF16.  Does not allow you
    // to distinguish null.
//    const String8 string8ObjectAt(size_t idx) const;//TODO;
#if 0
    const ResStringPool_span* styleAt(const ResStringPool_ref& ref) const;
    const ResStringPool_span* styleAt(size_t idx) const;

    ssize_t indexOfString(const char16_t* str, size_t strLen) const;

    size_t size() const;
    size_t styleCount() const;
    size_t bytes() const;

    bool isSorted() const;
    bool isUTF8() const;
#endif

private:
    status_t                    mError;
    void*                       mOwnedData;
    const ResStringPool_header* mHeader;
    size_t                      mSize;
    const uint32_t*             mEntries;           //over ResStringPool_header addr
    const uint32_t*             mEntryStyles;       // = mEntries + mHeader->stringCount
    const void*                 mStrings;          //sometimes = mHeader + mHeader->stringsStart
    char16_t mutable**          mCache;
    uint32_t                    mStringPoolSize;    // number of uint16_t
    const uint32_t*             mStyles;
    uint32_t                    mStylePoolSize;    // number of uint32_t
};


#endif	/* RESSTRINGPOOL_H */

