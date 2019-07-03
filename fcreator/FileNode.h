/* 
 * File:   FileNode.h
 * Author : angelToms
 *
 */

#ifndef FILENODE_H
#define	FILENODE_H

#include <string>
#include <vector>

struct file_node {
    struct file_node* parent;
    std::vector<struct file_node*> child;
    std::string oldName;
    std::string newName;
    bool isDir;
    bool isChanged;
    int deep;
};

typedef struct file_node FileNode;

struct deep_dir {
    std::vector<FileNode*> fnodes;
    int deep;//目录深度
};

typedef struct deep_dir DeepDir;

#endif	/* FILENODE_H */

