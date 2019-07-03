/* 
 * File:   ArscComplexParser.h
 * Author: angel-toms
 *
 */

#ifndef ARSC_COMPLEX_PARSER_H
#define	ARSC_COMPLEX_PARSER_H

#include "Parser.h"

#include "../ByteOrder.h"
#include "../ResIncludes.h"
#include "../dto/ResTab.h"

class ArscComplexParser : public Parser {
public:
    ArscComplexParser(const std::string filePath, ParseType type, bool copyData);
    virtual ~ArscComplexParser();
    
    virtual void* getParser() { return mResTab; }
    virtual bool parser();
    virtual void print(bool logAll, bool incValues);
    
private:
    ResTab* mResTab;

};

#endif	/* ARSC_COMPLEX_PARSER_H */

