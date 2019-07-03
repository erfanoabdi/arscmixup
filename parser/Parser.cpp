/* 
 * File:   Parser.cpp
 * Author : angelToms
 * 
 */

#include "Parser.h"

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <error.h>
#include <errno.h>
#include <string.h>

#include "../QupLog.h"

extern int errno;

Parser::Parser(const std::string filePath, ParseType type, bool copyData)
: mFilePath(filePath), mParseType(type), mData(NULL), mSize(0),
mCopyData(copyData) {

}

Parser::~Parser() {
    if (mData) {
        delete mData;
        mData = NULL;
    }
}

void* Parser::getParser() {
    return NULL;
}

bool Parser::loadDataFromFile() {
    const char* filePath = mFilePath.c_str();
    QUP_LOGI("[*] open file path = %s", filePath);

    struct stat stbuf;
    int fd = open(filePath, O_RDONLY);
    if (fd < 0) {
        QUP_LOGI("[-] open file %s fail, %s", filePath, strerror(errno));
        return false;
    }

    if (fstat(fd, &stbuf) < 0) {
        QUP_LOGI("[-] fstat file %d fail, %s", fd, strerror(errno));
        return false;
    }

    mSize = stbuf.st_size;
    size_t bufsize = mSize;
    
#if 0
    bufsize = mSize + (mSize / 1000) + 1;
#endif

    QUP_LOGI("[*] file size = %lu, buf size = %lu", mSize, bufsize);

    mData = new u1[bufsize];
    if (!mData) {
        QUP_LOGI("[-] calloc buf fail %s", strerror(errno));
        return false;
    }
    if (read(fd, mData, mSize) < 0) {
        QUP_LOGI("[-] read buf fail %s", strerror(errno));
        close(fd);
        return false;
    }

    close(fd);
    QUP_LOGI("[*] read buf over\n");

    return true;
}

