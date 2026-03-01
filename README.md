# 🧠 SIMPLELANG → 8-BIT CPU COMPILER

A 15-day execution plan for building a compiler targeting a custom 8-bit CPU.

## 🚀 Clean Pipeline to Run All

To run the entire end-to-end process (Build, Compile, Simulate), simply execute:
```bash
./run.sh
```
This script automates the full toolchain.

## 📂 Directory Structure
```text
.
├── src/               # Compiler Source Code (C++)
├── tests/             # Sample SimpleLang Programs
├── 8bit-computer/     # Target CPU Simulator (Verilog)
├── Makefile           # Build System
├── run.sh             # Automation Script
├── README.md          # Project Guidelines
└── error_log.md       # Debugging History
```

## 📌 Architectural Decisions

### Data Model
* **8-bit unsigned integers**: Range 0–255.
* **Overflow Policy**: Arithmetic wraps (0xFF + 1 = 0x00).

### Memory Model
* **Static Allocation**: Program code starts at `0x00`, variables start at `0x80`.
* **No Stack**: Uses a **register-machine** approach with registers `C-G` for expression temporaries.

## 📜 Formal Grammar (BNF)

```text
program        → declaration* statement*
declaration    → "int" IDENTIFIER ";"
statement      → assignment | if_statement
assignment     → IDENTIFIER "=" expression ";"
if_statement   → "if" "(" condition ")" "{" statement* "}"
condition      → expression "==" expression
expression     → term (("+"|"-") term)*
term           → factor
factor         → NUMBER | IDENTIFIER | "(" expression ")"
```

## ⚠️ Error Policies

### Lexical Errors
* **Unknown Character**: Print line number and character, then `exit(1)`.
* **Invalid Number**: Numbers > 255 will be truncated/wrapped (8-bit model).

### Syntax Errors
* **Unexpected Token**: Print "Expected [token] but found [token]" at line [N], then `exit(1)`.

## 📋 Register Mapping

| Register | Purpose |
|----------|---------|
| `r0 (A)` | Primary Accumulator / Left Operand |
| `r1 (B)` | Right Operand |
| `r2-r6`  | Intermediate results (Register Stack) |

## 📦 Memory Model
* **Program**: Starts at `0x00`.
* **Variables**: Dynamically allocated from `0x80` onwards.

## 📝 Example Translation

**SimpleLang:**
```c
int a;
a = 10;
if (a == 10) { a = 0; }
```

**Generated Assembly:**
```asm
.text
ldi A 10
sta 128
lda 128
mov B A
ldi A 10
cmp
jne %label_end_1
ldi A 0
sta 128
label_end_1:
hlt
```