/* 
 * File:   ArscMixer.h
 * Author: AngelToms
 *
 */

#ifndef ARSCMIXER_H
#define	ARSCMIXER_H

#include "Mixer.h"

#define RESOURCES_ARSC_NEW_NAME             "resources.arsc.1"

class ArscMixer : public Mixer {
public:
    ArscMixer(ParseType type, bool isCopy, bool logAll, bool log,
            bool onlyParse,
            const char* resourceDir, const char* resFileDir);
    virtual ~ArscMixer();

    bool mixer();

    virtual bool parser();
    virtual bool mixup();
    virtual bool saveNewFile();

private:

};

#endif	/* ARSCMIXER_H */

