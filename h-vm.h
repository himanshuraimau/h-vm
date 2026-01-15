/*
 * h-vm.h - H-VM Virtual Machine Header
 *
 * A 16-bit virtual machine with x86-inspired architecture
 */

#ifndef H_VM_H
#define H_VM_H

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include "h-utils.h"

#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#pragma GCC diagnostic push

/* ============================================================================
 * Type Definitions
 * ========================================================================= */

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;

/* Casting macros for type conversion */
#define $1 (int8 *)
#define $2 (int16)
#define $4 (int32)
#define $8 (int64)
#define $c (char *)
#define $i (int)

/* ============================================================================
 * Error Codes
 * ========================================================================= */

#define NoErr       0x00    /* No error */
#define SysHlt      0x01    /* System halted (normal exit) */
#define ErrMem      0x02    /* Memory allocation error */
#define ErrSegv     0x04    /* Segmentation fault */
#define ErrInstr    0x08    /* Illegal instruction */

typedef unsigned char Errorcode;

/* ============================================================================
 * Register Access Macros
 * ========================================================================= */

#define $ax ->c.r.ax
#define $bx ->c.r.bx
#define $cx ->c.r.cx
#define $dx ->c.r.dx
#define $sp ->c.r.sp
#define $ip ->c.r.ip
#define $flags ->c.r.flags

/* Flag testing macros */
#define equal(x)    (!!((x $flags & 0x08) >> 3))
#define gt(x)       (!!((x $flags & 0x04) >> 2))
#define higher(x)   (!!((x $flags & 0x02) >> 1))
#define lower(x)    (!!(x $flags & 0x01))

#define segfault(x) error((x), ErrSegv)

#endif /* H_VM_H */
