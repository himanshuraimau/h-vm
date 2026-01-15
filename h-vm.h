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
 * Register and CPU Structures
 * ========================================================================= */

/*
 * Architecture Overview:
 *   16-bit registers: AX, BX, CX, DX, SP, IP
 *   FLAGS register with condition flags
 *   65KB memory (full 16-bit address space)
 */

typedef unsigned short int Reg;

struct s_registers {
    Reg ax;     /* Accumulator */
    Reg bx;     /* Base register */
    Reg cx;     /* Counter register */
    Reg dx;     /* Data register */
    Reg sp;     /* Stack pointer */
    Reg ip;     /* Instruction pointer */
    Reg flags;  /* Status flags */
    /*
     * FLAGS layout:
     *   Bit 3: E - Equal flag
     *   Bit 2: G - Greater-than flag
     *   Bit 1: H - Higher byte flag
     *   Bit 0: L - Lower byte flag
     */
};
typedef struct s_registers Registers;

struct s_cpu {
    Registers r;
};
typedef struct s_cpu CPU;

/* ============================================================================
 * Memory and VM Structures
 * ========================================================================= */

typedef int8 Memory[((int16)(-1))];  /* 65KB memory */
typedef int8 Program;

struct s_vm {
    CPU c;
    Memory m;
    int16 b;    /* Break/program end pointer */
};
typedef struct s_vm VM;

typedef Memory *Stack;

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
