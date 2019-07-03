/* 
 * File:   FileNodeOp.h
 * Author : angelToms
 *
 */

#ifndef FILENODEOP_H
#define	FILENODEOP_H

#include "FileNode.h"
#include "FCreator.h"

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <map>

#include "../QupLog.h"
#include "../Types.h"

#define FILE_SPLIT              "/"
#define DEFAULT_BUF_SIZE        5

class FileNodeOp : public FCreator {
public:
    FileNodeOp(const char* dir);
    virtual ~FileNodeOp();

    virtual bool fcreate();
    void printNode(FileNode* node);
    virtual void printAll(bool log);

private:
    void initDirFileNodes(const char* dir);
    bool iterDir(const char* dir, int depth);
    void initDefaultValue(FileNode* fileNode);

    FileNode* makeRootFileNode();
    FileNode* createNullNode();
    DeepDir* createDeepDir();
    //通过深度遍历去查找或者通过map红黑树遍历
    FileNode* getFileNode(int deep, std::string name);
    std::string& getPath(FileNode* node);
    DeepDir* getDeepDir(int deep);

    void setNodeIsChanged(FileNode* node, bool isChanged);
    void setNodeNewName(FileNode* node, std::string name);
    void addChild(FileNode* node, FileNode* cnode);
    void setParent(FileNode* node, FileNode* pnode);
    int getChildSize(FileNode* node);
    bool isEqNode(FileNode* node1, FileNode* node2);

    std::string createListMapKey(int deep, std::string name);

    void printAllNode();
    void mixupName();
    bool createStringMap();
    bool renameDir();

private:
    char* mDir; //传人路径拷贝
    FileNode* mroot; //根(res)节点
    std::vector<DeepDir*> mDeepDirs; //按深度存储的节点
    int mDeep; //目录深度
    unsigned int mAllFileCount; //包含目录，所有文件数目
    unsigned int mFileCount; //不包含目录，所有文件数目

    std::map<std::string, FileNode*> mFindList; //为了便于查找, key = deep-fnode->oldName,基于红黑树查找快
    std::map<FileNode*, std::string> mFindPathList; //用于反向查找，value是res/xxx/xxx.xml这样的全路径
};

#endif	/* FILENODEOP_H */

