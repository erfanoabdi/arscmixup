/* 
 * File:   ResXMLTreeNode.h
 * Author : angelToms
 *
 */

#ifndef RESXMLTREENODE_H
#define	RESXMLTREENODE_H

#include "../ResChunkHead.h"
#include "../ResStringPoolRef.h"

/**
 * Basic XML tree node.  A single item in the XML document.  Extended info
 * about the node can be found after header.headerSize.
 */
struct ResXMLTree_node
{
    struct ResChunk_header header;

    // Line number in original source file at which this element appeared.
    uint32_t lineNumber;

    // Optional XML comment that was associated with this element; -1 if none.
    struct ResStringPool_ref comment;
};

#endif	/* RESXMLTREENODE_H */

