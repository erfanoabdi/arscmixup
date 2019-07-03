/* 
 * File:   ResTabPackage.h
 * Author : angelToms
 *
 */

#ifndef RESTABPACKAGE_H
#define	RESTABPACKAGE_H

#include <stdlib.h>
#include <vector>

#include "../ResIncludes.h"
#include "ResTabHeader.h"

struct Package;

struct Type
{
    Type(const Header* _header, const Package* _package, size_t count)
        : header(_header), package(_package), entryCount(count),
          typeSpec(NULL), typeSpecFlags(NULL) { }
    const Header* const             header;
    const Package* const            package;
    const size_t                    entryCount;
    const ResTable_typeSpec*        typeSpec;
    const uint32_t*                 typeSpecFlags;
    std::vector<const ResTable_type*>    configs;
};

struct Package
{
    Package(const Header* _header, const ResTable_package* _package)
        :  header(_header), package(_package) { }
    ~Package()
    {
        size_t i = types.size();
        while (i > 0) {
            i--;
            delete types[i];
        }
    }
    
    const Header* const             header;
    const ResTable_package* const   package;
    std::vector<Type*>              types;

    ResStringPool                   typeStrings;
    ResStringPool                   keyStrings;
    
    const Type* getType(size_t idx) const {
        return idx < types.size() ? types[idx] : NULL;
    }
};

#endif	/* RESTABPACKAGE_H */

