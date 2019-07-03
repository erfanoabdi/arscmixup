/* 
 * File:   FCreator.cpp
 * Author: AngelToms
 * 
 */

#include "FCreator.h"

FCreator::FCreator(const char* dir) : mdir(dir) {
}

FCreator::~FCreator() {
    std::map<ResMaps*, ResMaps*>::iterator iter;
    for (iter = mStringMap.begin(); iter != mStringMap.end(); iter++) {
        ResMaps* oldMap = iter->first;
        delete oldMap;
        oldMap = NULL;

        ResMaps* newMap = iter->second;
        delete newMap;
        newMap = NULL;
    }
    
    mStringMap.clear();
    mStringMaps.clear();
}

std::map<ResMaps*, ResMaps*>& FCreator::getStringMap() {
    return mStringMap;
}

std::map<std::string, std::string>& FCreator::getStringMaps() {
    return mStringMaps;
}

