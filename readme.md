# H-VM - 16-bit Virtual Machine

A simple 16-bit virtual machine with x86-inspired architecture, written in C.

## Features

- **6 General Purpose Registers**: AX, BX, CX, DX, SP, IP
- **FLAGS Register**: Equal, Greater-than, Higher, Lower flags
- **65KB Memory**: Full 16-bit addressable memory space
- **Stack Operations**: PUSH and POP support
- **Basic Opcodes**: NOP, HLT, MOV, flag operations

## Architecture

```
Registers (16-bit each):
  AX - Accumulator
  BX - Base register
  CX - Counter register
  DX - Data register
  SP - Stack pointer (starts at 0xFFFF)
  IP - Instruction pointer

FLAGS:
  Bit 5: C - Carry flag (arithmetic overflow/underflow)
  Bit 4: Z - Zero flag (result is zero)
  Bit 3: E - Equal flag
  Bit 2: G - Greater-than flag
  Bit 1: H - Higher byte flag
  Bit 0: L - Lower byte flag
```

## Opcodes

| Opcode | Mnemonic | Description |
|--------|----------|-------------|
| 0x01 | NOP | No operation |
| 0x02 | HLT | Halt execution |
| 0x08-0x0f | MOV | Move data to registers |
| 0x10 | STE | Set equal flag |
| 0x11 | CLE | Clear equal flag |
| 0x12 | STG | Set greater-than flag |
| 0x13 | CLG | Clear greater-than flag |
| 0x14 | STH | Set higher flag |
| 0x15 | CLH | Clear higher flag |
| 0x16 | STL | Set lower flag |
| 0x17 | CLL | Clear lower flag |
| 0x1a | PUSH | Push register to stack |
| 0x1b | POP | Pop stack to register |
| **0x20** | **ADD** | **Add value to register** |
| **0x21** | **SUB** | **Subtract value from register** |
| **0x22** | **MUL** | **Multiply register by value** |
| **0x23** | **DIV** | **Divide register by value** |
| **0x24** | **INC** | **Increment register** |
| **0x25** | **DEC** | **Decrement register** |

## Building

```bash
make        # Build the VM
make clean  # Clean build artifacts
```

## Running

```bash
./h-vm
```

### Example Output

```
vm   = 0x583041d3c2a0 (sz: 65552)
prog = 0x583041d3c2bc
System halted
ax = 0004
bx = 0004
sp = ffff
E flag set
0000000000000000000000000000000000000000000000000000000000000400
```

## Example Program

The included example program demonstrates basic VM operations:

```c
mov ax, 0x04    // Move 0x04 to AX register
ste             // Set equal flag
push ax         // Push AX to stack
mov bx, 0x5005  // Move 0x5005 to BX register
pop bx          // Pop stack to BX (overwrites with 0x04)
hlt             // Halt execution
```

## Project Structure

```
h-vm/
├── h-vm.h      # Header with types, structures, declarations
├── h-vm.c      # Implementation
├── h-utils.h   # Utility functions (zero, copy, printhex)
├── Makefile    # Build configuration
└── readme.md   # This file
```

## Implementation Details

- **Memory**: 65KB (full 16-bit address space)
- **Stack**: Grows downward from 0xFFFF
- **Instruction Format**: Variable length (1-5 bytes)
- **Error Handling**: Segmentation faults, illegal instructions

## License

MIT

