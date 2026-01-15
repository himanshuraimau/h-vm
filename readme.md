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
  Bit 3: E - Equal flag
  Bit 2: G - Greater-than flag
  Bit 1: H - Higher byte flag
  Bit 0: L - Lower byte flag
```

## Building

```bash
make        # Build the VM
make clean  # Clean build artifacts
```

## Running

```bash
./h-vm
```

## Project Structure

```
h-vm/
├── h-vm.h      # Header with types, structures, declarations
├── h-vm.c      # Implementation
├── h-utils.h   # Utility functions
├── Makefile    # Build configuration
└── readme.md   # This file
```

## License

MIT
