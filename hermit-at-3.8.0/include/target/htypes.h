/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 *
 * __s8 and friends copied from linux/asm/types.h
 *      - yashi Feb. 06, 2004
 */

#ifndef _HERMIT_TARGET_HTYPES_H_
#define _HERMIT_TARGET_HTYPES_H_

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
typedef __signed__ long long __s64;
typedef unsigned long long __u64;
#endif

typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;
typedef __u64 __le64;
typedef __u64 __be64;

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

#define BITS_PER_LONG 32

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef unsigned long size_t;
typedef size_t addr_t;
typedef size_t word_t;

#ifndef NULL
#define NULL ((void *)0)
#endif

/* for 32 bit target; add #ifdef here if adding 64 bit support */
#define UNALIGNED_MASK (3)

/* number of bits to right-shift for byte count => word count */
#define BYTE_TO_WORD_SHIFT (2)

#endif /* _HERMIT_TARGET_HTYPES_H_ */
