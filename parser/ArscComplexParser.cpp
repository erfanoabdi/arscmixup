/*
 * File:   ArscComplexParser.cpp
 * Author: angel-toms
 *
 */

#include "ArscComplexParser.h"

#include "../QupLog.h"
#include "../dto/ResTabHeader.h"
#include "../dto/ResTabPackage.h"
#include "../utils/Debug.h"

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>
//#include <errors.h>

ArscComplexParser::ArscComplexParser(const std::string filePath, ParseType type, bool copyData)
: Parser(filePath, type, copyData),
mResTab(NULL) {
}

ArscComplexParser::~ArscComplexParser() {
    delete mResTab;
    mResTab = NULL;
}

bool ArscComplexParser::parser() {
    u1* p = mData;

    mResTab = new ResTab(mData, mSize, mCopyData);
    if(!mResTab) {
        QUP_LOGI("[-] alloc res table fail");
        return false;
    }

    if(!mResTab->makeResTab()) {
        QUP_LOGI("[-] make res table fail");
        return false;
    }

    return true;
}

void ArscComplexParser::print(bool logAll, bool inclValues) {
    mResTab->printResTab(logAll, inclValues);
}
