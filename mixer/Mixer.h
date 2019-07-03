/* 
 * File:   Mixer.h
 * Author: AngelToms
 *
 */

#ifndef MIXER_H
#define	MIXER_H

#include "../parser/Parser.h"
#include "../fcreator/FCreator.h"

#include <map>
#include <string>

class Mixer {
public:
    Mixer(ParseType type, bool isCopy, bool logAll,
            bool log, bool onlyParse,
            const char* resourceDir, const char* resFileDir);
    virtual ~Mixer();

    virtual bool mixer() = 0;

    std::map<ResMaps*, ResMaps*>& getStringMap() {
        return mCreator->getStringMap();
    }

protected:
    virtual bool parser() = 0;
    virtual bool mixup() = 0;
    virtual bool saveNewFile() = 0;

protected:
    Parser* mParser;
    FCreator* mCreator;

    ParseType mParseType;
    bool mIsCopy;
    bool mLogAll;
    bool mLog;
    bool mOnlyParse;
    const char* mResourceDir; //对应 resources.arsc 或者 mainfest.xml路径
    const char* mResFileDir; //如果是resources.arsc则为Res文件对应目录，如果为mainfest则为空

};

#endif	/* MIXER_H */

