/* 
 * File:   FileScan.h
 * Author : angelToms
 *
 */

#ifndef FILESCAN_H
#define	FILESCAN_H

#include <unistd.h>  
#include <stdio.h>
#include <stdlib.h>

#ifdef	__cplusplus
extern "C" {
#endif

    bool iter_dir(const char* dir, int depth, int* deep);

    bool iter_del_dir(const char* dir, int depth);

#ifdef	__cplusplus
}
#endif

#endif	/* FILESCAN_H */

