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
 * MOV Instruction
 * ========================================================================= */

/*
 * __mov - Move data to registers
 * @vm: VM instance
 * @opcode: Specific MOV variant (0x08-0x0f)
 * @a1: Source value or address
 * @a2: Unused for most variants
 *
 * Supports:
 *   - Full 16-bit register moves
 *   - High byte (H flag) and low byte (L flag) operations
 *   - Memory address operations (placeholders)
 */
void __mov(VM *vm, Opcode opcode, Args a1, Args a2) {
    int16 dst;

    dst = $2 a1;
    if (higher(vm) && lower(vm))
        error(vm, ErrInstr);

    switch (opcode) {
        /* mov ax - 0x08 */
        case 0x08:
            if (higher(vm))
                vm $ax = (((Reg)a1 << 8) | (vm $ax & 0xFF));
            else if (lower(vm))
                vm $ax = ((Reg)a1 | (vm $ax & 0xFF00));
            else
                vm $ax = (Reg)a1;
            break;

        /* mov bx - 0x09 */
        case 0x09:
            if (higher(vm))
                vm $bx = (((Reg)a1 << 8) | (vm $bx & 0xFF));
            else if (lower(vm))
                vm $bx = ((Reg)a1 | (vm $bx & 0xFF00));
            else
                vm $bx = (Reg)a1;
            break;

        /* mov cx - 0x0a */
        case 0x0a:
            if (higher(vm))
                vm $cx = (((Reg)a1 << 8) | (vm $cx & 0xFF));
            else if (lower(vm))
                vm $cx = ((Reg)a1 | (vm $cx & 0xFF00));
            else
                vm $cx = (Reg)a1;
            break;

        /* mov dx - 0x0b */
        case 0x0b:
            if (higher(vm))
                vm $dx = (((Reg)a1 << 8) | (vm $dx & 0xFF));
            else if (lower(vm))
                vm $dx = ((Reg)a1 | (vm $dx & 0xFF00));
            else
                vm $dx = (Reg)a1;
            break;

        /* mov sp - 0x0c */
        case 0x0c:
            vm $sp = (Reg)dst;
            break;

        /* mov [addr],ax - 0x0d */
        case 0x0d:
            break;

        /* mov [addr],bx - 0x0e */
        case 0x0e:
            break;

        /* mov [addr],dx - 0x0f */
        case 0x0f:
            break;

        default:
            error(vm, ErrInstr);
            break;
    }

    return;
}

/* ============================================================================
 * Stack Operations
 * ========================================================================= */

/*
 * __push - Push register value onto stack
 * @vm: VM instance
 * @opcode: PUSH opcode
 * @a1: Register selector (0x00=ax, 0x01=bx, 0x02=cx, 0x03=dx)
 * @a2: Unused
 *
 * Stack grows downward from 0xFFFF
 * Checks for stack overflow and flag conflicts
 */
void __push(VM *vm, Opcode opcode, Args a1, Args a2) {
    int16 src;
    int16 *dst;
    void *mem;

    if (higher(vm) || lower(vm))
        error(vm, ErrInstr);
    if (vm $sp < 2)
        error(vm, ErrInstr);
    if (vm $sp < (vm->b - 2))
        error(vm, ErrSegv);

    switch (a1) {
        /* push ax = 0x00 */
        case 0x00:
            src = vm $ax;
            break;
        /* push bx = 0x01 */
        case 0x01:
            src = vm $bx;
            break;
        /* push cx = 0x02 */
        case 0x02:
            src = vm $cx;
            break;
        /* push dx = 0x03 */
        case 0x03:
            src = vm $dx;
            break;
        default:
            error(vm, ErrInstr);
    }

    vm $sp -= 2;
    mem = vm->m + vm $sp;
    dst = mem;
    *dst = src;

    return;
}

/*
 * __pop - Pop value from stack into register
 * @vm: VM instance
 * @opcode: POP opcode
 * @a1: Register selector (0x00=ax, 0x01=bx, 0x02=cx, 0x03=dx)
 * @a2: Unused
 *
 * Checks for stack underflow and flag conflicts
 */
void __pop(VM *vm, Opcode opcode, Args a1, Args a2) {
    int16 *src;
    void *mem;

    if (higher(vm) || lower(vm))
        error(vm, ErrInstr);
    if (vm $sp > 0xfffd)
        error(vm, ErrInstr);

    mem = vm->m + vm $sp;
    src = mem;

    switch (a1) {
        /* pop ax = 0x00 */
        case 0x00:
            vm $ax = *src;
            break;
        /* pop bx = 0x01 */
        case 0x01:
            vm $bx = *src;
            break;
        /* pop cx = 0x02 */
        case 0x02:
            vm $cx = *src;
            break;
        /* pop dx = 0x03 */
        case 0x03:
            vm $dx = *src;
            break;
        default:
            error(vm, ErrInstr);
    }
    vm $sp += 2;

    return;
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

/* ============================================================================
 * Execution Engine
 * ========================================================================= */

/*
 * execinstr - Execute a single instruction
 * @vm: VM instance
 * @p: Pointer to instruction in memory
 *
 * Parses instruction arguments and dispatches to appropriate handler
 */
void execinstr(VM* vm, Program *p) {
    Args a1, a2;
    int16 size;

    a1 = a2 = 0;
    size = map(*p);

    switch (size) {
        case 1:
            break;

        case 2:
            a1 = *(p+1);
            break;

        case 3:
            a1 = (
                (((int16)*(p+2) & 0xff) << 8)
                    | ((int16)*(p+1) & 0xff)
            );
            break;

        case 5:
            a1 = (
                (((int16)*(p+2) & 0xff) << 8)
                    | ((int16)*(p+1) & 0xff)
            );

            a2 = (
                (((int16)*(p+4) & 0xff) << 8)
                    | ((int16)*(p+3) & 0xff)
            );
            break;

        default:
            segfault(vm);
            break;
    }

    switch (*p) {
        case mov: 
        case 0x09 ... 0x0f:
            __mov(vm, (Opcode)*p, a1, a2);
            break;

        case nop:
            break;
        
        case hlt:
            error(vm, SysHlt);
            break;

        case ste:    __ste(vm, (Opcode)*p, a1, a2); break;
        case stg:    __stg(vm, (Opcode)*p, a1, a2); break;
        case sth:    __sth(vm, (Opcode)*p, a1, a2); break;
        case stl:    __stl(vm, (Opcode)*p, a1, a2); break;
        case cle:    __cle(vm, (Opcode)*p, a1, a2); break;
        case clg:    __clg(vm, (Opcode)*p, a1, a2); break;
        case clh:    __clh(vm, (Opcode)*p, a1, a2); break;
        case cll:    __cll(vm, (Opcode)*p, a1, a2); break;
        case push:  __push(vm, (Opcode)*p, a1, a2); break;
        case pop:    __pop(vm, (Opcode)*p, a1, a2); break;
    }

    return;
}

/*
 * execute - Main execution loop
 * @vm: VM instance
 *
 * Executes instructions from memory until HLT is encountered
 */
void execute(VM *vm) {
    int32 brkaddr;
    Program *pp;
    int16 size;

    assert(vm && *vm->m);
    size = 0;
    brkaddr = ((int32)vm->m + vm->b);
    pp = (Program *)&vm->m;

    do {
        vm $ip += size;
        pp += size;

        if ((int32)pp > brkaddr)
            segfault(vm);
        size = map(*pp);
        execinstr(vm, pp);
    } while (*pp != (Opcode)hlt);

    return;
}

/* ============================================================================
 * Main Entry Point
 * ========================================================================= */

/*
 * main - Entry point with example program
 *
 * Example program:
 *   mov ax, 0x04
 *   ste              ; Set equal flag
 *   push ax
 *   mov bx, 0x5005
 *   pop bx
 *   hlt
 */
int main(int argc, char *argv[]) {
    Program *prog;
    VM *vm;

    vm = virtualmachine();
    prog = exampleprogram(vm,
        i(i1(mov, 0x04)), i(i0(ste)), i(i1(push, 0x00)),
        i(i1(0x09, 0x5005)), i(i1(pop, 0x01)),
        
        
        i(i0(hlt)) 
    );
    printf("vm   = %p (sz: %d)\n", vm, sizeof(struct s_vm));
    printf("prog = %p\n", prog);

    execute(vm);

    printhex($1 prog, (map(mov)+map(nop)+map(hlt)), ' ');

    return 0;
}

#pragma GCC diagnostic pop