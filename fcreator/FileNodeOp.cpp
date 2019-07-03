/* 
 * File:   FileNodeOp.cpp
 * Author : angelToms
 * 
 */

#include "FileNodeOp.h"
#include "AsciiLet.h"

#include <stack>
#include <dirent.h>
#include <string.h>  
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <string.h>
#include <errno.h>
#include<stdio.h>
#include<stdlib.h>

static std::string nullString = std::string("Unknow");

FileNodeOp::FileNodeOp(const char* dir) : FCreator(dir), mDir(NULL), mroot(NULL),
mDeep(0), mAllFileCount(0), mFileCount(0) {
    int dirLen = strlen(dir) + 1;
    mDir = new char[dirLen];
    if (!mDir) {
        QUP_LOGI("[-] calloc new dir string fail");
        exit(-1);
    }
    memset(mDir, 0, dirLen);
    memcpy(mDir, dir, (dirLen - 1));

    std::string dirstring(mDir);
    int idx = dirstring.rfind(FILE_SPLIT);
    mParentDir = dirstring.substr(0, idx);

    mroot = makeRootFileNode();
    if (!mroot) {
        exit(-1);
    }

    DeepDir* deepDir = createDeepDir();
    if (!deepDir) {
        exit(-1);
    }
    deepDir->deep = 0;
    deepDir->fnodes.push_back(mroot);

    mDeepDirs.push_back(deepDir);

    mAllFileCount++;

    std::string key = createListMapKey(mroot->deep, mroot->oldName);
    mFindList[key] = mroot;

    QUP_LOGI("[*] dir = %s, parent dir = %s", mDir, mParentDir.c_str());

    initDirFileNodes(mDir);
}

FileNodeOp::~FileNodeOp() {
    delete mDir;
    mDir = NULL;

    int size = mDeepDirs.size();
    for (int i = (size - 1); i > -1; i--) {
        DeepDir* dpDir = mDeepDirs[i];
        for (int j = 0; j < dpDir->fnodes.size(); j++) {
            FileNode* f = dpDir->fnodes[j];
            delete f;
            f = NULL;
        }
        dpDir->fnodes.clear();
        delete dpDir;
        dpDir = NULL;
    }

    mDeepDirs.clear();
    mFindList.clear();
    mFindPathList.clear();
}

std::string FileNodeOp::createListMapKey(int deep, std::string name) {
    if (deep > 127) {
        QUP_LOGI("[-] deep %d too big, not support");
        exit(-1);
    }

    char buf[DEFAULT_BUF_SIZE];
    memset(buf, 0, DEFAULT_BUF_SIZE);
    sprintf(buf, "%d", deep);

    std::string key(buf);
    key.append("-");
    key.append(name);

    return key;
}

void FileNodeOp::initDefaultValue(FileNode* fileNode) {
    fileNode->parent = NULL;
    fileNode->child = std::vector<FileNode*>();
    fileNode->isChanged = false;
    fileNode->isDir = false;
    fileNode->oldName = nullString;
    fileNode->newName = nullString;
    fileNode->deep = -1;
}

FileNode* FileNodeOp::createNullNode() {
    FileNode* fnode = new FileNode();
    if (!fnode) {
        QUP_LOGI("[-] alloc null file node fail");
        return NULL;
    }
    initDefaultValue(fnode);
    return fnode;
}

DeepDir* FileNodeOp::createDeepDir() {
    DeepDir* deepDir = new DeepDir();
    if (!deepDir) {
        QUP_LOGI("[-] calloc deep dir instance fail");
        return NULL;
    }

    deepDir->deep = -1;
    deepDir->fnodes = std::vector<FileNode*>();

    return deepDir;
}

FileNode* FileNodeOp::getFileNode(int deep, std::string name) {
#ifndef DEEP_SEARCH 
    //基于map的红黑树搜索，适合数量集大的
    std::string key = createListMapKey(deep, name);
    std::map<std::string, FileNode*>::iterator iter;

    iter = mFindList.find(key);

    if (iter != mFindList.end()) {
        return iter->second;
    } else {
        return NULL;
    }
#else
    //深度迭代搜索，速度比较慢，适合少量
    for (int i = 0; i < mDeepDirs.size(); i++) {
        DeepDir* deepDir = mDeepDirs[i];
        if (deepDir->deep == deep) {
            for (int j = 0; j < deepDir->fnodes.size(); j++) {
                FileNode* fnode = deepDir->fnodes[j];
                if (fnode->oldName.compare(name) == 0) {
                    return fnode;
                }
            }
        }
        continue;
    }
    return NULL;
#endif
}

std::string& FileNodeOp::getPath(FileNode* node) {
    static std::string ret("");
    std::map<FileNode*, std::string>::iterator iter;
    iter = mFindPathList.find(node);

    if (iter != mFindPathList.end()) {
        return iter->second;
    } else {
        return ret;
    }
}

DeepDir* FileNodeOp::getDeepDir(int deep) {
    for (int i = 0; i < mDeepDirs.size(); i++) {
        DeepDir* dp = mDeepDirs[i];
        if (dp->deep == deep) {
            return dp;
        }
    }
    return NULL;
}

bool FileNodeOp::isEqNode(FileNode* node1, FileNode* node2) {
    if (node1->oldName.compare(node2->oldName) == 0 && node1->deep == node2->deep) {
        return true;
    }
    return false;
}

FileNode* FileNodeOp::makeRootFileNode() {
    mroot = createNullNode();
    if (!mroot) {
        return NULL;
    }

    mroot->deep = 0;
    mroot->isDir = true;
    mroot->oldName = std::string("res");

    return mroot;
}

void FileNodeOp::addChild(FileNode* node, FileNode* cnode) {
    node->child.push_back(cnode);
}

void FileNodeOp::setParent(FileNode* node, FileNode* pnode) {
    node->parent = pnode;
}

int FileNodeOp::getChildSize(FileNode* node) {
    return node->child.size();
}

void FileNodeOp::setNodeIsChanged(FileNode* node, bool isChanged) {
    node->isChanged = isChanged;
}

void FileNodeOp::setNodeNewName(FileNode* node, std::string name) {
    node->newName = name;
}

void FileNodeOp::printNode(FileNode* node) {
    QUP_LOGI("[*] node info :"
            " node parent name : %s"
            " node is dir : %d"
            " node is changed : %d"
            " node deep = %d"
            " node old name : %s"
            " node new name : %s",
            node->parent->oldName.empty() ? "none" : node->parent->oldName.c_str(),
            node->isDir,
            node->isChanged,
            node->deep,
            node->oldName.c_str(),
            node->newName.c_str());
}

void FileNodeOp::initDirFileNodes(const char* dir) {
    if (!iterDir(dir, 0)) {
        exit(-1);
    }
}

bool FileNodeOp::iterDir(const char* dir, int depth) {
    std::string dirstring(dir);
    int idx = dirstring.rfind(FILE_SPLIT) + 1;
    std::string fparent = dirstring.substr(idx);

    if (mDeep < depth)
        mDeep = depth;

    FileNode* parent = getFileNode(mDeep, fparent);
    if (!parent) {
        QUP_LOGI("[-] find file node deep = %d, name = %s fail", mDeep, fparent.c_str());
        return false;
    }

    int realDeep = mDeep + 1;

#ifdef DEBUG_ALL
    QUP_LOGI("[*] deep = %d, depth = %d, real deep = %d", mDeep, depth, realDeep);
#endif

    DeepDir* dpDir = getDeepDir(realDeep);
    if (!dpDir) {
        dpDir = createDeepDir();
        if (!dpDir) {
            return false;
        }
        dpDir->deep = realDeep;
        mDeepDirs.push_back(dpDir); //如果目录下是空的，则同样会多这么一个深度
    }

    DIR* dp;
    struct dirent* entry;
    struct stat statbuf;

    if ((dp = opendir(dir)) == NULL) {
#ifdef DEBUG_ALL
        QUP_LOGI("[-] can't open dir %s, %s", copyDir.c_str(), strerror(errno));
#endif
        return false;
    }

    chdir(dir);

    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
        if (S_IFDIR & statbuf.st_mode) {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;

            FileNode* fdir = createNullNode();
            if (!fdir) {
                return false;
            }
            fdir->deep = realDeep;
            fdir->isDir = true;
            fdir->oldName = std::string(entry->d_name);
            setParent(fdir, parent);
            addChild(parent, fdir);

            dpDir->fnodes.push_back(fdir);
            mAllFileCount++;

            std::string key = createListMapKey(fdir->deep, fdir->oldName);
            mFindList[key] = fdir;

#ifdef DEBUG_ALL
            QUP_LOGI("[*] depth = %d, dir  name = %s", realDeep, entry->d_name);
#endif
            if (!iterDir(entry->d_name, depth + 1)) {
                return false;
            }
        } else {
            FileNode* file = createNullNode();
            if (!file) {
                return false;
            }
            file->deep = realDeep;
            file->oldName = std::string(entry->d_name);
            setParent(file, parent);
            addChild(parent, file);

            dpDir->fnodes.push_back(file);
            mAllFileCount++;
            mFileCount++;

            std::string key = createListMapKey(file->deep, file->oldName);
            mFindList[key] = file;

#ifdef DEBUG_ALL
            QUP_LOGI("[*] depth = %d, file name = %s", realDeep, entry->d_name);
#endif
        }
    }

    chdir("..");
    closedir(dp);

    return true;
}

bool FileNodeOp::createStringMap() {
    static std::vector<std::string> ofile;
    static std::vector<std::string> nfile;
    static std::string ofullName("");
    static std::string nfullName("");
    static std::string spl(FILE_SPLIT);

    int size = mDeepDirs.size();
    for (int i = (size - 1); i > -1; i--) {
        DeepDir* dpDir = mDeepDirs[i];
        for (int j = 0; j < dpDir->fnodes.size(); j++) {
            FileNode* cp = dpDir->fnodes[j];
            FileNode* f = dpDir->fnodes[j];
            ofile.push_back(f->oldName);
            nfile.push_back(f->newName);
            for (int k = i - 1; k >= 0; k--) {
                f = f->parent;
                if (f) {
                    ofile.push_back(f->oldName);
                    nfile.push_back(f->newName);
                }
            }
            for (int ij = ofile.size() - 1; ij >= 0; ij--) {
                if (ij != 0) {
                    ofullName.append(ofile[ij]);
                    ofullName.append(spl);
                    nfullName.append(nfile[ij]);
                    nfullName.append(spl);
                } else {
                    ofullName.append(ofile[ij]);
                    nfullName.append(nfile[ij]);
                }
            }

            ResMaps* resOldMap = new ResMaps();
            ResMaps* resNewMap = new ResMaps();

            if (!resOldMap || !resNewMap) {
                QUP_LOGI("[-] calloc res map fail");
                return false;
            }

            resOldMap->isDir = cp->isDir;
            resOldMap->name = ofullName;
            resNewMap->isDir = cp->isDir;
            resNewMap->name = nfullName;

            mStringMap[resOldMap] = resNewMap;
            mStringMaps[ofullName] = nfullName;
            mFindPathList[cp] = ofullName;

#ifdef DEBUG_ALL
            QUP_LOGI("[*] %s - %s", ofullName.c_str(), nfullName.c_str());
            QUP_LOGI("[*] %s", ofullName.c_str());
            QUP_LOGI("[*] %s", nfullName.c_str());
#endif
            ofullName.clear();
            ofile.clear();
            nfullName.clear();
            nfile.clear();
        }
    }

    return true;
}

void FileNodeOp::mixupName() {
    static char buf[DEFAULT_BUF_SIZE];
    int size = mDeepDirs.size();
    for (int i = 0; i < size; i++) {
        DeepDir* dpDir = mDeepDirs[i];
        for (int j = 0; j < dpDir->fnodes.size(); j++) {
            memset(buf, 0, DEFAULT_BUF_SIZE);
            nameCreator(buf, j);
            FileNode* f = dpDir->fnodes[j];
            int idx = f->oldName.find(".");
            if (idx != -1) {
                std::string suf = f->oldName.substr(idx);
                f->newName = std::string(buf).append(suf);
            } else {
                f->newName = std::string(buf);
            }
            f->isChanged = true;
        }
    }
}

bool FileNodeOp::renameDir() {
    //需要从最深的节点开始到根节点重命名
    int size = mDeepDirs.size();
    for (int i = (size - 1); i > -1; i--) {
        DeepDir* dpDir = mDeepDirs[i];
        for (int j = 0; j < dpDir->fnodes.size(); j++) {
            FileNode* f = dpDir->fnodes[j];
            std::string& path = getPath(f);
            if (path.empty()) {
                QUP_LOGI("[-] find deep = %d, name = %s fail", f->deep, f->oldName.c_str());
                return false;
            }
            std::string oldName = mParentDir;
            oldName.append(FILE_SPLIT).append(path);

            int idx = oldName.rfind(FILE_SPLIT) + 1;
            std::string newName = oldName.substr(0, idx);
            newName.append(f->newName);

#ifdef DEBUG_ALL
            QUP_LOGI("[*] old name = %s, new name = %s", oldName.c_str(), newName.c_str());
#endif

            if (rename(oldName.c_str(), newName.c_str()) < 0) {
                QUP_LOGI("[-] rename %s to %s fail, reason %s", strerror(errno));
                return false;
            }
        }
    }

    return true;
}

void FileNodeOp::printAllNode() {
    std::map<std::string, std::string>::iterator iter;
    for (iter = mStringMaps.begin(); iter != mStringMaps.end(); iter++) {
        QUP_LOGI("[*] %s - %s", iter->first.c_str(), iter->second.c_str());
    }
}

void FileNodeOp::printAll(bool log) {
    if (log) {
        printAllNode();
    }
}

bool FileNodeOp::fcreate() {
    mixupName();
    createStringMap();
    if (!renameDir()) {
        return false;
    }

    return true;
}