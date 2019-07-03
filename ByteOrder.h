/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//

#ifndef _LIBS_UTILS_BYTE_ORDER_H
#define _LIBS_UTILS_BYTE_ORDER_H

#include <stdint.h>
#include <sys/types.h>
#include <errno.h>
#ifdef HAVE_WINSOCK
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

#include "utils/Errors.h"

#ifndef INT32_MAX
#define INT32_MAX ((int32_t)(2147483647))
#endif

#define STRING_POOL_NOISY(x) //x
#define XML_NOISY(x) //x
#define TABLE_NOISY(x) //x
#define TABLE_GETENTRY(x) //x
#define TABLE_SUPER_NOISY(x) //x
#define LOAD_TABLE_NOISY(x) //x
#define TABLE_THEME(x) //x

#ifdef HAVE_WINSOCK
#undef  nhtol
#undef  htonl

#ifdef HAVE_LITTLE_ENDIAN
#define ntohl(x)    ( ((x) << 24) | (((x) >> 24) & 255) | (((x) << 8) & 0xff0000) | (((x) >> 8) & 0xff00) )
#define htonl(x)    ntohl(x)
#define ntohs(x)    ( (((x) << 8) & 0xff00) | (((x) >> 8) & 255) )
#define htons(x)    ntohs(x)
#else
#define ntohl(x)    (x)
#define htonl(x)    (x)
#define ntohs(x)    (x)
#define htons(x)    (x)
#endif
#endif

#define IDMAP_MAGIC         0x706d6469

/*
 * These macros are like the hton/ntoh byte swapping macros,
 * except they allow you to swap to and from the "device" byte
 * order.  The device byte order is the endianness of the target
 * device -- for the ARM CPUs we use today, this is little endian.
 *
 * Note that the byte swapping functions have not been optimized
 * much; performance is currently not an issue for them since the
 * intent is to allow us to avoid byte swapping on the device.
 */

static inline uint32_t android_swap_long(uint32_t v)
{
    return (v<<24) | ((v<<8)&0x00FF0000) | ((v>>8)&0x0000FF00) | (v>>24);
}

static inline uint16_t android_swap_short(uint16_t v)
{
    return (v<<8) | (v>>8);
}

#define DEVICE_BYTE_ORDER LITTLE_ENDIAN

#if BYTE_ORDER == DEVICE_BYTE_ORDER

#define	dtohl(x)	(x)
#define	dtohs(x)	(x)
#define	htodl(x)	(x)
#define	htods(x)	(x)

#else

#define	dtohl(x)	(android_swap_long(x))
#define	dtohs(x)	(android_swap_short(x))
#define	htodl(x)	(android_swap_long(x))
#define	htods(x)	(android_swap_short(x))

#endif

#if BYTE_ORDER == LITTLE_ENDIAN
#define fromlel(x) (x)
#define fromles(x) (x)
#define tolel(x) (x)
#define toles(x) (x)
#else
#define fromlel(x) (android_swap_long(x))
#define fromles(x) (android_swap_short(x))
#define tolel(x) (android_swap_long(x))
#define toles(x) (android_swap_short(x))
#endif

#endif // _LIBS_UTILS_BYTE_ORDER_H
