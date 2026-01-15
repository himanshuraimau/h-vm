/*
 * h-vm.c - H-VM Virtual Machine Implementation
 *
 * A 16-bit virtual machine with x86-inspired architecture
 */

#include "h-vm.h"

/* ============================================================================
 * Flag Operations
 * ========================================================================= */

/*
 * Flag operations manipulate the FLAGS register bits:
 *   Bit 3: E - Equal flag
 *   Bit 2: G - Greater-than flag
 *   Bit 1: H - Higher byte flag
 *   Bit 0: L - Lower byte flag
 */

void __ste(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags |= 0x08;
}

void __stg(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags |= 0x04;
}

void __sth(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags |= 0x02;
}

void __stl(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags |= 0x01;
}

void __cle(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags &= 0x07;
}

void __clg(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags &= 0x0c;
}

void __clh(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags &= 0x0d;
}

void __cll(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags &= 0x0e;
}


/* ============================================================================
 * VM Core Functions
 * ========================================================================= */

/*
 * virtualmachine - Allocate and initialize a new VM
 * Returns: Pointer to initialized VM, or NULL on error
 */
VM *virtualmachine(void) {
    VM *p;
    int16 size;

    size = $2 sizeof(struct s_vm);
    p = (VM *)malloc($i size);
    if (!p) {
        errno = ErrMem;
        return (VM *)0;
    }
    zero($1 p, size);
    p $sp = 0xffff;  /* Stack starts at top of memory */

    return p;
}

/*
 * map - Get instruction size from opcode
 * @o: Opcode to look up
 * Returns: Size of instruction in bytes, or 0 if not found
 */
int8 map(Opcode o) {
    int8 n, ret;
    IM *p;

    ret = 0;
    for (n = IMs, p = instrmap; n; n--, p++)
        if (p->o == o) {
            ret = p->s;
            break;
        }
    
    return ret;
}

/*
 * error - Handle VM errors and exit
 * @vm: VM instance
 * @e: Error code
 */
void error(VM* vm, Errorcode e) {
    int8 exitcode;

    exitcode = -1;
    switch(e) {
        case ErrSegv:
            fprintf(stderr, "%s\n", "VM Segmentation fault");
            break;

        case ErrInstr:
            fprintf(stderr, "%s\n", "VM Illegal instruction");
            break;

        case SysHlt:
            fprintf(stderr, "%s\n", "System halted");
            exitcode = 0;
            printf("ax = %.04hx\n", $i vm $ax);
            printf("bx = %.04hx\n", $i vm $bx);
            printf("sp = %.04hx\n", $i vm $sp);
            if (equal(vm))
                printf("E flag set\n");
            if (gt(vm))
                printf("GT flag set\n");
            
            printhex(vm->m + 0xffff - 32, 32, 0);
            break;

        default:
            break;
    }
    if (vm)
        free(vm);

    exit($i exitcode);
}

/* ============================================================================
 * Instruction Builder Functions
 * ========================================================================= */

/*
 * i0 - Create instruction with no arguments
 * @op: Opcode
 * Returns: Pointer to instruction
 */
Instruction *i0(Opcode op) {
    Instruction *i;
    int8 size;

    size = map(op);
    i = (Instruction *)malloc(size);
    assert(i);
    zero($1 i, size);
    i->o = op;

    return i;
}

/*
 * i1 - Create instruction with one argument
 * @op: Opcode
 * @a1: First argument
 * Returns: Pointer to instruction
 */
Instruction *i1(Opcode op, Args a1) {
    Instruction *i;
    int8 size;

    size = map(op);
    i = (Instruction *)malloc(size);
    assert(i);
    zero($1 i, size);
    i->o = op;
    i->a[0] = a1;

    return i;
}

/*
 * i2 - Create instruction with two arguments
 * @op: Opcode
 * @a1: First argument
 * @a2: Second argument
 * Returns: Pointer to instruction
 */
Instruction *i2(Opcode op, Args a1, Args a2) {
    Instruction *i;
    int8 size;

    size = map(op);
    i = (Instruction *)malloc(size);
    assert(i);
    zero($1 i, size);
    i->o = op;
    i->a[0] = a1;
    i->a[1] = a2;

    return i;
}

/*
 * i - Convert instruction to program bytes
 * @i: Instruction to convert
 * Returns: Pointer to program bytes
 */
Program *i(Instruction *i) {
    Program *p;
    int8 size;

    size = map(i->o);
    p = (Program *)malloc($i size);
    assert(p);
    copy(p, $1 i, 1);
    copy((p+1), $1 i->a, (size-1));

    return p;
}

/*
 * exampleprogram - Load variadic program into VM memory
 * @vm: VM instance
 * @...: Variable number of Program* pointers, ending with hlt
 * Returns: Pointer to end of program
 */
Program *exampleprogram(VM *vm, ...) {
    Program *p, *instr;
    va_list ap;
    Opcode opc;
    int8 size;

    va_start(ap, vm);
    p = vm->m;

    do {
        instr = va_arg(ap, Program*);
        opc = (Opcode)*instr;
        size = map(opc);
        copy($1 p, $1 instr, size);
        p += size;
        vm->b += size;
    } while(opc != hlt);
    va_end(ap);

    return p;
}

#pragma GCC diagnostic pop