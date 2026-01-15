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

/* Arithmetic flag macros */
#define zero_flag(x)  (!!((x $flags & 0x10) >> 4))
#define carry_flag(x) (!!((x $flags & 0x20) >> 5))

/*
 * Extended FLAGS register layout:
 *   Bit 5: C - Carry flag (arithmetic overflow/underflow)
 *   Bit 4: Z - Zero flag (result is zero)
 *   Bit 3: E - Equal flag
 *   Bit 2: G - Greater-than flag
 *   Bit 1: H - Higher byte flag
 *   Bit 0: L - Lower byte flag
 */

#define segfault(x) error((x), ErrSegv)

/* ============================================================================
 * Opcodes and Instructions
 * ========================================================================= */

enum e_opcode {
    nop  = 0x01,
    hlt  = 0x02,
    mov  = 0x08,    /* 0x08 - 0x0f */
    ste  = 0x10,    /* Set equal flag */
    cle  = 0x11,    /* Clear equal flag */
    stg  = 0x12,    /* Set greater-than flag */
    clg  = 0x13,    /* Clear greater-than flag */
    sth  = 0x14,    /* Set higher flag */
    clh  = 0x15,    /* Clear higher flag */
    stl  = 0x16,    /* Set lower flag */
    cll  = 0x17,    /* Clear lower flag */
    push = 0x1a,
    pop  = 0x1b
};
typedef enum e_opcode Opcode;

/* Instruction map structure - maps opcodes to instruction sizes */
struct s_instrmap {
    Opcode o;
    int8 s;
};
typedef struct s_instrmap IM;

/* Instruction structure with flexible array for arguments */
typedef int16 Args;
struct s_instruction {
    Opcode o;
    Args a[];   /* 0-2 bytes */
};
typedef struct s_instruction Instruction;

/* Static instruction map - defines size of each opcode */
static IM instrmap[] = {
    { nop,  0x01 },
    { hlt,  0x01 },
    { mov,  0x03 },
        { 0x09, 0x03 }, { 0x0a, 0x03 }, { 0x0b, 0x03 }, { 0x0c, 0x03 },
        { 0x0d, 0x03 }, { 0x0e, 0x03 }, { 0x0f, 0x03 },
    { ste,  0x01 },
    { stg,  0x01 },
    { stl,  0x01 },
    { sth,  0x01 },
    { cle,  0x01 },
    { clg,  0x01 },
    { cll,  0x01 },
    { clh,  0x01 },
    { push, 0x03 },
    { pop,  0x03 }
};
#define IMs (sizeof(instrmap) / sizeof(struct s_instrmap))

/* ============================================================================
 * Function Declarations
 * ========================================================================= */

/* Flag operations */
void __ste(VM*, Opcode, Args, Args);
void __stg(VM*, Opcode, Args, Args);
void __sth(VM*, Opcode, Args, Args);
void __stl(VM*, Opcode, Args, Args);
void __cle(VM*, Opcode, Args, Args);
void __clg(VM*, Opcode, Args, Args);
void __clh(VM*, Opcode, Args, Args);
void __cll(VM*, Opcode, Args, Args);

/* Stack operations */
void __push(VM*, Opcode, Args, Args);
void __pop(VM*, Opcode, Args, Args);

/* MOV instruction */
void __mov(VM*, Opcode, Args, Args);

/* Core VM functions */
void error(VM*, Errorcode);
void execinstr(VM*, Program*);
void execute(VM*);
Program *i(Instruction*);
Instruction *i0(Opcode);
Instruction *i1(Opcode, Args);
Instruction *i2(Opcode, Args, Args);
int8 map(Opcode);
VM *virtualmachine(void);
int main(int, char**);


#endif /* H_VM_H */
