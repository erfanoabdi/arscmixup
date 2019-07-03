/* 
 * File:   Mixer.cpp
 * Author: AngelToms
 * 
 */

#include "Mixer.h"

Mixer::Mixer(ParseType type, bool isCopy, bool logAll, bool log,
        bool onlyParse, const char* resourceDir, const char* resFileDir) :
mParseType(type), mIsCopy(isCopy), mLogAll(logAll), mLog(log), mOnlyParse(onlyParse),
mResourceDir(resourceDir), mResFileDir(resFileDir),
mParser(NULL), mCreator(NULL) {
}

Mixer::~Mixer() {
    delete mParser;
    mParser = NULL;
    delete mCreator;
    mCreator = NULL;
}

