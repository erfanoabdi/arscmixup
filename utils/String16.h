/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_STRING16_H
#define ANDROID_STRING16_H

#include "Errors.h"
#include "SharedBuffer.h"
#include "Unicode.h"
#include "TypeHelpers.h"

// ---------------------------------------------------------------------------

extern "C" {

}

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

class String8;
class TextOutput;

//! This is a string holding UTF-16 characters.
class String16
{
public:
    /* use String16(StaticLinkage) if you're statically linking against
     * libutils and declaring an empty static String16, e.g.:
     *
     *   static String16 sAStaticEmptyString(String16::kEmptyString);
     *   static String16 sAnotherStaticEmptyString(sAStaticEmptyString);
     */
    enum StaticLinkage { kEmptyString };

                                String16();
    explicit                    String16(StaticLinkage);
                                String16(const String16& o);
                                String16(const String16& o,
                                         size_t len,
                                         size_t begin=0);
    explicit                    String16(const uint16_t* o);
    explicit                    String16(const uint16_t* o, size_t len);
    explicit                    String16(const String8& o);
    explicit                    String16(const char* o);
    explicit                    String16(const char* o, size_t len);

                                ~String16();
    
    inline  const uint16_t*     string() const;
    inline  size_t              size() const;
    
    inline  const SharedBuffer* sharedBuffer() const;
    
            void                setTo(const String16& other);
            status_t            setTo(const uint16_t* other);
            status_t            setTo(const uint16_t* other, size_t len);
            status_t            setTo(const String16& other,
                                      size_t len,
                                      size_t begin=0);
    
            status_t            append(const String16& other);
            status_t            append(const uint16_t* other, size_t len);
            
    inline  String16&           operator=(const String16& other);
    
    inline  String16&           operator+=(const String16& other);
    inline  String16            operator+(const String16& other) const;

            status_t            insert(size_t pos, const uint16_t* chrs);
            status_t            insert(size_t pos,
                                       const uint16_t* chrs, size_t len);

            ssize_t             findFirst(uint16_t c) const;
            ssize_t             findLast(uint16_t c) const;

            bool                startsWith(const String16& prefix) const;
            bool                startsWith(const uint16_t* prefix) const;
            
            status_t            makeLower();

            status_t            replaceAll(uint16_t replaceThis,
                                           uint16_t withThis);

            status_t            remove(size_t len, size_t begin=0);

    inline  int                 compare(const String16& other) const;

    inline  bool                operator<(const String16& other) const;
    inline  bool                operator<=(const String16& other) const;
    inline  bool                operator==(const String16& other) const;
    inline  bool                operator!=(const String16& other) const;
    inline  bool                operator>=(const String16& other) const;
    inline  bool                operator>(const String16& other) const;
    
    inline  bool                operator<(const uint16_t* other) const;
    inline  bool                operator<=(const uint16_t* other) const;
    inline  bool                operator==(const uint16_t* other) const;
    inline  bool                operator!=(const uint16_t* other) const;
    inline  bool                operator>=(const uint16_t* other) const;
    inline  bool                operator>(const uint16_t* other) const;
    
    inline                      operator const uint16_t*() const;
    
private:
            const uint16_t*     mString;
};

// String16 can be trivially moved using memcpy() because moving does not
// require any change to the underlying SharedBuffer contents or reference count.
ANDROID_TRIVIAL_MOVE_TRAIT(String16)

// ---------------------------------------------------------------------------
// No user servicable parts below.

inline int compare_type(const String16& lhs, const String16& rhs)
{
    return lhs.compare(rhs);
}

inline int strictly_order_type(const String16& lhs, const String16& rhs)
{
    return compare_type(lhs, rhs) < 0;
}

inline const uint16_t* String16::string() const
{
    return mString;
}

inline size_t String16::size() const
{
    return SharedBuffer::sizeFromData(mString)/sizeof(uint16_t)-1;
}

inline const SharedBuffer* String16::sharedBuffer() const
{
    return SharedBuffer::bufferFromData(mString);
}

inline String16& String16::operator=(const String16& other)
{
    setTo(other);
    return *this;
}

inline String16& String16::operator+=(const String16& other)
{
    append(other);
    return *this;
}

inline String16 String16::operator+(const String16& other) const
{
    String16 tmp(*this);
    tmp += other;
    return tmp;
}

inline int String16::compare(const String16& other) const
{
    return strzcmp16(mString, size(), other.mString, other.size());
}

inline bool String16::operator<(const String16& other) const
{
    return strzcmp16(mString, size(), other.mString, other.size()) < 0;
}

inline bool String16::operator<=(const String16& other) const
{
    return strzcmp16(mString, size(), other.mString, other.size()) <= 0;
}

inline bool String16::operator==(const String16& other) const
{
    return strzcmp16(mString, size(), other.mString, other.size()) == 0;
}

inline bool String16::operator!=(const String16& other) const
{
    return strzcmp16(mString, size(), other.mString, other.size()) != 0;
}

inline bool String16::operator>=(const String16& other) const
{
    return strzcmp16(mString, size(), other.mString, other.size()) >= 0;
}

inline bool String16::operator>(const String16& other) const
{
    return strzcmp16(mString, size(), other.mString, other.size()) > 0;
}

inline bool String16::operator<(const uint16_t* other) const
{
    return strcmp16(mString, other) < 0;
}

inline bool String16::operator<=(const uint16_t* other) const
{
    return strcmp16(mString, other) <= 0;
}

inline bool String16::operator==(const uint16_t* other) const
{
    return strcmp16(mString, other) == 0;
}

inline bool String16::operator!=(const uint16_t* other) const
{
    return strcmp16(mString, other) != 0;
}

inline bool String16::operator>=(const uint16_t* other) const
{
    return strcmp16(mString, other) >= 0;
}

inline bool String16::operator>(const uint16_t* other) const
{
    return strcmp16(mString, other) > 0;
}

inline String16::operator const uint16_t*() const
{
    return mString;
}
// ---------------------------------------------------------------------------

#endif // ANDROID_STRING16_H
