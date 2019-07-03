/* 
 * File:   PackageGroup.h
 * Author : angelToms
 *
 */

#ifndef PACKAGEGROUP_H
#define	PACKAGEGROUP_H

#include <stdlib.h>
#include <vector>

#include "ResTabPackage.h"
#include "../QupLog.h"
#include "../utils/String16.h"

struct bag_set
{
    size_t numAttrs;    // number in array
    size_t availAttrs;  // total space in array
    uint32_t typeSpecFlags;
    // Followed by 'numAttr' bag_entry structures.
};

// A group of objects describing a particular resource package.
// The first in 'package' is always the root object (from the resource
// table that defined the package); the ones after are skins on top of it.
struct PackageGroup
{
    PackageGroup( const String16& _name, uint32_t _id)
        :  name(_name), id(_id), typeCount(0), bags(NULL) { }
    ~PackageGroup() {
        clearBagCache();
        const size_t N = packages.size();
        for (size_t i=0; i<N; i++) {
            Package* pkg = packages[i];
              delete pkg;
        }
    }

    void clearBagCache() {
        if (bags) {
            QUP_LOGI("[*] bags=%p\n", bags);
            Package* pkg = packages[0];
            QUP_LOGI("[*] typeCount=%x\n", typeCount);
            for (size_t i=0; i<typeCount; i++) {
                QUP_LOGI("[*] type=%d\n", i);
                const Type* type = pkg->getType(i);
                if (type != NULL) {
                    bag_set** typeBags = bags[i];
                    QUP_LOGI("[*] typeBags=%p\n", typeBags);
                    if (typeBags) {
                        QUP_LOGI("[*] type->entryCount=%x\n", type->entryCount);
                        const size_t N = type->entryCount;
                        for (size_t j=0; j<N; j++) {
                            if (typeBags[j] && typeBags[j] != (bag_set*)0xFFFFFFFF)
                                free(typeBags[j]);
                        }
                        free(typeBags);
                    }
                }
            }
            free(bags);
            bags = NULL;
        }
    }
    
    String16 const                  name;
    uint32_t const                  id;
    std::vector<Package*>                packages;
    
    // This is for finding typeStrings and other common package stuff.
    Package*                        basePackage;

    // For quick access.
    size_t                          typeCount;
    
    // Computed attribute bags, first indexed by the type and second
    // by the entry in that type.
    bag_set***                      bags;
};

#endif	/* PACKAGEGROUP_H */

