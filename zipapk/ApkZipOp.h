/* 
 * File:   ApkZipOp.h
 * Author: angeltoms
 *
 */

#ifndef APKZIPOP_H
#define	APKZIPOP_H

#include <stdlib.h>
#include <string>
#include <map>

#include "../mixer/Mixer.h"
#include "../zip/ZipFile.h"

class ApkZipOp {
public:
    ApkZipOp(std::string fname, bool isBack = false);
    virtual ~ApkZipOp();
    
    bool unzipResToPwd();
    
    bool delApkRes();
    
    bool addApkMixupRes(std::map<ResMaps*, ResMaps*>& maps);
    
    std::string& getUnzipPath() {
        return mUnzipPath;
    }
    
private:
    std::string mFileName;
    std::string mUnzipPath;
    bool mIsBack;//是否备份
    
};

#endif	/* APKZIPOP_H */

