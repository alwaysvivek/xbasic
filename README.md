# 🧠 XBASIC-MODERN → 8-BIT CPU COMPILER

A powerful hybrid language merging XBasic syntax with a high-performance C++ compiler and Verilog-based 8-bit CPU.

## 🚀 Installation & Usage

To install the XBasic-Modern toolset:
```bash
pip install .
```
After installation, you can run any `.sl` program using the unified CLI:
```bash
xb-modern tests/loop.sl
```

**Output:**
```text
> 15
```

Need more detail? Use the debug flag:
```bash
xb-modern tests/loop.sl --debug
```

## 📂 Directory Structure
```text
.
├── xbasic_modern/     # Python Package
│   ├── compiler/     # C++ Source
│   ├── simulator/     # Verilog RTL & Assembler
│   └── tests/         # Sample Programs
├── setup.py           # Packaging Script
└── README.md          # Project Specifications
```

## 📜 Language Specification (XBasic-Modern)

### Data Types
* **num**: 8-bit unsigned integer (0-255).
* **text**: String literal support (Experimental).

### Control Flow
* **IF...THEN...ELSE...END**: Standard conditional logic.
* **FOR...TO...NEXT**: Iterative loops with automatic increment.
* **WHILE...END**: Condition-based loops.
* **PRINT**: Hardware-mapped output.

### Formal Grammar (BNF)
```text
program        → statement*
statement      → declaration | assignment | if_st | for_st | while_st | print_st
declaration    → ("num" | "text") IDENTIFIER ["=" expression]
assignment     → IDENTIFIER "=" expression
if_st          → "IF" condition "THEN" statement* ["ELSE" statement*] "END"
for_st         → "FOR" IDENTIFIER "=" expression "TO" expression statement* "NEXT"
while_st       → "WHILE" condition statement* "END"
print_st       → "PRINT" expression
condition      → expression ("==" | "!=" | ">" | "<") expression
expression     → term (("+"|"-") term)*
term           → factor (("*"|"/") factor)*
factor         → NUMBER | IDENTIFIER | STRING | "(" expression ")"
```

## 📋 Register Mapping

| Register | Purpose |
|----------|---------|
| `r0 (A)` | Primary Accumulator / ALU Result |
| `r1 (B)` | Secondary Operand |
| `r2-r6`  | General Purpose / Scratch |
| `r7 (T)` | Memory Address Buffer |

## 📦 Memory Model
* **Program**: Starts at `0x00`.
* **Variables**: Statically allocated starting at `0x80`.

## 📝 Example Code
```xbasic
num i = 0
num sum = 0
FOR i = 1 TO 5
  sum = sum + i
NEXT
PRINT sum
```
**Output:** `15 ($0f)`