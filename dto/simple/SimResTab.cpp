/* 
 * File:   SimResTab.cpp
 * Author : angelToms
 * 
 */

#include "SimResTab.h"

#include "../../QupLog.h"
#include "../../ByteOrder.h"
#include "../ResTabHeader.h"

SimResTab::SimResTab(u1* data, size_t size, bool isCopy) : mData(data), mSize(size),
mIsMakeable(false), mIsCopy(isCopy), mHeader(NULL), mStringPool(NULL) {
}

SimResTab::~SimResTab() {
    if (mStringPool) {
        delete mStringPool;
        mStringPool = NULL;
    }
    if (mPackages.size() > 0) {
        for (int i = 0; i < mPackages.size(); i++) {
            SimResPackage* pkg = mPackages[i];
            delete pkg;
            pkg = NULL;
        }
        mPackages.clear();
    }
}

bool SimResTab::makeSimResTab() {
    u1* p = mData;
    //ResTable
    mHeader = (struct ResTable_header*) p;
    printResTable(mHeader);

    p += sizeof (ResTable_header);

    //ResStringPool
    mStringPool = new SimResStringPool(p, mIsCopy);
    if (!mStringPool->makeResStringPool()) {
        return false;
    }

    p += mStringPool->mStringPoolHeader->header.size;

    //ResPackage, 可能包含N个
    for (int i = 0; i < mHeader->packageCount; i++) {
        SimResPackage* pkg = new SimResPackage(p, mIsCopy, mHeader, mStringPool);
        if (!pkg) {
            QUP_LOGI("[-] calloc pkg %d fail, not enought memory!");
            return false;
        }
        if (!pkg->makeResPackage()) {
            return false;
        }

        p += pkg->getPkgSize();
        mPackages.push_back(pkg);
    }

    mIsMakeable = true;
    return true;
}

void SimResTab::printAll(bool logAll, bool isAll) {
    if (!mIsMakeable)
        makeSimResTab();

    if (logAll) {
        if (isAll)
            mStringPool->printStringPool();

        int pkgSize = mPackages.size();
        QUP_LOGI("[*] resPackage size = %d", pkgSize);
        for (int i = 0; i < pkgSize; i++) {
            SimResPackage* pkg = mPackages[i];
            pkg->printPkgString(isAll);
        }
    }
}

SimResStringPool* SimResTab::getSimResStringPool() {
    return mStringPool;
}