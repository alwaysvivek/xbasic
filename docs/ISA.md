# 8-bit CPU Instruction Set Architecture (ISA)

The target CPU is an 8-bit machine with 256 bytes of RAM and 8 general-purpose registers.

## Registers
- `A` (000): Accumulator / Left Operand / Result
- `B` (001): Right Operand
- `C` (010)
- `D` (011)
- `E` (100)
- `F` (101)
- `G` (110)
- `M` (111): Memory Address Pointer (Pseudo-register)

## Data Transfer Instructions
- `ldi r D`: Load 8-bit immediate `D` into register `r`. (2 bytes)
- `mov r1 r2`: Copy value from `r2` to `r1`. (1 byte)
- `lda addr`: Load value from `addr` into `A`. (2 bytes)
- `sta addr`: Store value from `A` into `addr`. (2 bytes)
- `mov M r addr`: Store register `r` into `addr`. (2 bytes)
- `mov r M addr`: Load `addr` into register `r`. (2 bytes)

## Arithmetic instructions
- `add`: A = A + B. (1 byte)
- `sub`: A = A - B. (1 byte)
- `inc`: A = A + 1. (1 byte)
- `dec`: A = A - 1. (1 byte)
- `cmp`: A - B (flags only). (1 byte)

## Branching instructions
- `jmp D`: Unconditional jump to `D`. (2 bytes)
- `je D`: Jump to `D` if Zero flag is set (A == B). (2 bytes)
- `jne D`: Jump to `D` if Zero flag is NOT set (A != B). (2 bytes)

## Machine Control
- `hlt`: Stop execution. (1 byte)
- `nop`: No operation. (1 byte)
