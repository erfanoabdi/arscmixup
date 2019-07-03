/* 
 * File:   ResXMLTreeHeader.h
 * Author : angelToms
 *
 */

#ifndef RESXMLTREEHEADER_H
#define	RESXMLTREEHEADER_H

#include "../ResChunkHead.h"

/**
 * XML tree header.  This appears at the front of an XML tree,
 * describing its content.  It is followed by a flat array of
 * ResXMLTree_node structures; the hierarchy of the XML document
 * is described by the occurrance of RES_XML_START_ELEMENT_TYPE
 * and corresponding RES_XML_END_ELEMENT_TYPE nodes in the array.
 */
struct ResXMLTree_header
{
    struct ResChunk_header header;
};

#endif	/* RESXMLTREEHEADER_H */

