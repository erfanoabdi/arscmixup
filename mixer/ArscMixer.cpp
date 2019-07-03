/* 
 * File:   ArscMixer.cpp
 * Author: AngelToms
 * 
 */

#include "ArscMixer.h"

#include "../parser/ArscComplexParser.h"
#include "../parser/ArscSimpleParser.h"
#include "../fcreator/FileNodeOp.h"

#include "../ResIncludes.h"
#include "../QupLog.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

ArscMixer::ArscMixer(ParseType type, bool isCopy, bool logAll,
        bool log, bool onlyParse,
        const char* resourceDir, const char* resFileDir) :
Mixer(type, isCopy, logAll, log, onlyParse, resourceDir, resFileDir) {
}

ArscMixer::~ArscMixer() {
}

bool ArscMixer::mixer() {
    QUP_LOGI("[*] Begin to mix up ...");
#ifdef ONLY_PARSE_NOT_MIXUP
    //此段只做解析，不做mixup用，此段代码是反正安卓源码实现的，打印详细解析resourcs.arsc的内容
    //如果混淆时，切勿打开此开关，如果仅仅是解析，而不调用后面的mixup，可以打开此开关，来查看解析结果
    mParser = new ArscComplexParser(std::string(mResourceDir),
            mParseType,
            mIsCopy);
#else
    mParser = new ArscSimpleParser(std::string(mResourceDir),
            mParseType,
            mIsCopy);
#endif

    if (!mParser) {
        QUP_LOGI("[-] calloc parser fail");
        return false;
    }

    QUP_LOGI("[*] Begin to parser resource");
    if (!parser()) {
        return false;
    }

    if (!mOnlyParse) {
        QUP_LOGI("[*] Begin to mixup  resource");
        mCreator = new FileNodeOp(mResFileDir);
        if (!mCreator) {
            QUP_LOGI("[-] calloc creator fail");
            return false;
        }
        if (!mixup()) {
            return false;
        }

        QUP_LOGI("[*] Begin to save   resource");
        if (!saveNewFile()) {
            return false;
        }
    }

    QUP_LOGI("[*] End   to mix up ...");
    return true;
}

bool ArscMixer::parser() {

#ifdef ONLY_PARSE_NOT_MIXUP
    //此段只做解析，不做mixup用，此段代码是反正安卓源码实现的，打印详细解析resourcs.arsc的内容
    //如果混淆时，切勿打开此开关，如果仅仅是解析，而不调用后面的mixup，可以打开此开关，来查看解析结果
    if (!mParser->loadDataFromFile()) {
        return false;
    }

    if (!mParser->parser()) {
        return false;
    }

    mParser->print(mLogAll, mLog);
#else

    if (!mParser->loadDataFromFile()) {
        return false;
    }

    if (!mParser->parser()) {
        return false;
    }
    mParser->print(mLogAll, mLog);
#endif

    return true;
}

bool ArscMixer::mixup() {

    if (!mCreator->fcreate()) {
        return false;
    }

    mCreator->printAll(mLog);

    //mixup
    SimResTab* resTab = (SimResTab*) mParser->getParser();
    SimResStringPool* stringPool = resTab->getSimResStringPool();
    stringPool->mixupStringPool(mCreator->getStringMaps());
    return true;
}

bool ArscMixer::saveNewFile() {
    std::string sdir = mCreator->getParentDir();
    sdir.append("/").append(RESOURCES_ARSC_NEW_NAME);

    QUP_LOGI("[*] new resfile name = %s", sdir.c_str());

    int fod = open(sdir.c_str(), O_CREAT | O_RDWR, 0755);
    if (fod < 0) {
        QUP_LOGI("[-] create file %s fail : %s", sdir.c_str(), strerror(errno));
        return false;
    }

    write(fod, mParser->getDataBase(), mParser->getResourceSize());
    fsync(fod);

    close(fod);
    return true;
}
