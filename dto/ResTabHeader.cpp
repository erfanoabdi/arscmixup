#include "ResTabHeader.h"

#include "../QupLog.h"

#include <stdint.h>

status_t validate_chunk(const ResChunk_header* chunk,
        size_t minSize,
        const uint8_t* dataEnd,
        const char* name) {
    const uint16_t headerSize = dtohs(chunk->headerSize);
    const uint32_t size = dtohl(chunk->size);

    if (headerSize >= minSize) {
        if (headerSize <= size) {
            if (((headerSize | size)&0x3) == 0) {
                if ((ssize_t) size <= (dataEnd - ((const uint8_t*) chunk))) {
                    return NO_ERROR;
                }
                QUP_LOGI("[-] %s data size %p extends beyond resource end %p.",
                        name, (void*) size,
                        (void*) (dataEnd - ((const uint8_t*) chunk)));
                return BAD_TYPE;
            }
            QUP_LOGI("[-] %s size 0x%x or headerSize 0x%x is not on an integer boundary.",
                    name, (int) size, (int) headerSize);
            return BAD_TYPE;
        }
        QUP_LOGI("[-] %s size %p is smaller than header size %p.",
                name, (void*) size, (void*) (int) headerSize);
        return BAD_TYPE;
    }
    QUP_LOGI("[-] %s header size %p is too small.",
            name, (void*) (int) headerSize);
    return BAD_TYPE;
}


