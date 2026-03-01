# Reimplementation and Error Log

## 🚀 Reimplementation Insights
If reimplementing from scratch, focus on these areas:
1. **Instruction Encoding**: The `asm.py` in the target repo has specific bit-packing for `mov` and `ldi`. Ensure the compiler's codegen matches this exactly.
2. **Register-ALU Coupling**: The ALU is hard-wired to `A` and `B`. All binary operations must preload these registers.
3. **Static Memory**: Since there is no stack, every variable needs a unique address. A simple counter starting from `0x10` works well.

## 🐞 Error Log

| Error | Cause | Resolution |
|-------|-------|------------|
| `std::format` not found | `Makefile` was using `-std=c++17`. | Updated `Makefile` to `-std=c++20`. |
| `Lexer.cpp` compilation failure | Incomplete `if (std::isalpha(c))` block. | Stubbed out the block with a default return. |
| Memory output not visible | `machine.v` only displays I/O port writes. | Use `$display` or GTKWave to inspect RAM if needed. |
