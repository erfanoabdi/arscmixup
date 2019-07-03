/* 
 * File:   Parser.h
 * Author : angelToms
 *
 */

#ifndef PARSER_H
#define	PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "../Types.h"
#include "../Common.h"

class Parser {
public:
    Parser(const std::string filePath, ParseType type, bool copyData);
    virtual ~Parser();

    virtual void* getParser();
    bool loadDataFromFile();
    virtual bool parser() = 0;
    virtual void print(bool logAll, bool incValues) = 0;
    
    u1* getDataBase() { return mData; }
    size_t getResourceSize() { return mSize; }
    ParseType getParserType() { return mParseType; }
    
protected:
    u1* mData;
    size_t mSize;
    
    ParseType mParseType;
    std::string mFilePath;
    bool mCopyData;
};

#endif	/* PARSER_H */

