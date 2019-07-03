/* 
 * File:   ArscSimpleParser.cpp
 * Author : angelToms
 * 
 */

#include "ArscSimpleParser.h"

#include <string.h>

ArscSimpleParser::ArscSimpleParser(const std::string filePath, ParseType type, bool copyData)
: Parser(filePath, type, copyData), mResTab(NULL) {
}

ArscSimpleParser::~ArscSimpleParser() {
    delete mResTab;
    mResTab = NULL;
}

bool ArscSimpleParser::parser() {
    mResTab = new SimResTab(mData, mSize, mCopyData);
    if (!mResTab) {
        QUP_LOGI("[-] calloc res tab fail");
        return false;
    }

    return mResTab->makeSimResTab();
}

void ArscSimpleParser::print(bool logAll, bool incValues) {
    mResTab->printAll(logAll, incValues);
}
