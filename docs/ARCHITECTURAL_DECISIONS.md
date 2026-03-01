# Data Model
- 8-bit unsigned integers (0-255)
- Arithmetic wraps on overflow
- No floats
- No signed integers

# Memory Model
- Static allocation; Each variable has a fixed memory address at compile time.

# Out of Scope
- No stack
- No recursion
- No frames
- No loops
- No functions
- No arrays
- No optimization

# Error policy
Lexer: Unknown character -> Print Line -> Char -> Exit
Parser:  Unexpected token -> print token + Line -> exit
semantic:redeclaration -> error

undeclared variable -> error