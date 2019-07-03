/* 
 * File:   ArscSimpleParser.h
 * Author : angelToms
 *
 */

#ifndef ARSCSIMPLEPARSER_H
#define	ARSCSIMPLEPARSER_H

#include "Parser.h"

#include "../ByteOrder.h"
#include "../ResIncludes.h"
#include "../dto/ResTab.h"
#include "../dto/simple/SimResTab.h"

class ArscSimpleParser : public Parser {
public:
    ArscSimpleParser(const std::string filePath, ParseType type, bool copyData);
    virtual ~ArscSimpleParser();

    virtual void*
    getParser() {
        return mResTab;
    }

    virtual bool parser();
    virtual void print(bool logAll, bool incValues);
    

private:
    SimResTab* mResTab;

};

#endif	/* ARSCSIMPLEPARSER_H */

