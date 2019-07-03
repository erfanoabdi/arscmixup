/* 
 * File:   AsciiLet.h
 * Author: Angel-Toms
 *
 */

#ifndef ASCIILET_H
#define	ASCIILET_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ONE_DIMENSION               26
#define TWO_DIMENSION              (26*26)
#define TWO_DIMENSION_BOUND         (TWO_DIMENSION + ONE_DIMENSION)
#define THREE_DIMENSION             (26*26*26)
#define THREE_DIMENSION_BOUND       (TWO_DIMENSION_BOUND + THREE_DIMENSION)
#define MAX_DIMENSION               32000

#ifdef	__cplusplus
extern "C" {
#endif

    const char let[] = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
    };

    inline void oneDimension(char* buf, size_t idx) {
        memcpy((void*) buf, &let[idx], 1);
    }

    inline void twoDimension(char* buf, size_t idx) {
        int i = (idx / ONE_DIMENSION);
        int n = (idx % ONE_DIMENSION);
        memcpy((void*) buf, &let[i], 1);
        memcpy((void*) ((char*) buf + 1), &let[n], 1);
    }

    inline void threeDimension(char* buf, size_t idx) {
        int i = idx / TWO_DIMENSION;
        int n = (idx % TWO_DIMENSION);
        twoDimension(buf, n);
        memcpy(buf + 2, &let[i], 1);
    }

    inline void nameCreator(char* buf, size_t idx) {
        if (idx < ONE_DIMENSION) {
            oneDimension(buf, idx);
            return;
        }

        if (idx >= ONE_DIMENSION && idx < TWO_DIMENSION_BOUND) {
            idx = idx - ONE_DIMENSION;
            twoDimension(buf, idx);
            return;
        }

        if (idx >= TWO_DIMENSION_BOUND && idx < THREE_DIMENSION_BOUND) {
            idx = idx - TWO_DIMENSION_BOUND;
            threeDimension(buf, idx);
            return;
        }
        if (idx >= THREE_DIMENSION_BOUND) {
            printf("[*] %u too big, not support", idx);
            exit(-1);
        }
        return;
    }
#ifdef	__cplusplus
}
#endif

#endif	/* ASCIILET_H */

