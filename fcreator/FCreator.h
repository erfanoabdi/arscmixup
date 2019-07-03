/* 
 * File:   FCreator.h
 * Author: AngelToms
 *
 */

#ifndef FCREATOR_H
#define	FCREATOR_H

#include "../Common.h"
#include "../Types.h"

#include <string>
#include <map>

typedef struct {
    std::string name;
    bool isDir;
} ResMaps;

class FCreator {
public:
    FCreator(const char* dir);
    virtual ~FCreator();

    std::map<ResMaps*, ResMaps*>& getStringMap();
    std::map<std::string, std::string>& getStringMaps();
    
    virtual bool fcreate() = 0;
    virtual void printAll(bool log) = 0;

    std::string getParentDir() {
        return mParentDir;
    }

protected:
    //下面两个map的中的名称字段是完全一样的
    std::map<ResMaps*, ResMaps*> mStringMap; //新旧文件对应关系,包含是否是目录，key为旧名称，value为新的
    std::map<std::string, std::string> mStringMaps; //新旧文件对应关系，key为旧名称，value为新的

protected:
    std::string mParentDir; //传人路径的父目录

private:
    const char* mdir;

};

#endif	/* FCREATOR_H */

