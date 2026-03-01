#!/bin/bash
set -e

# 1. Build the compiler
echo "--- Step 1: Building Compiler ---"
make clean && make

# 2. Compile SimpleLang to Assembly
echo "--- Step 2: Compiling tests/input.sl ---"
mkdir -p tests
./compiler tests/input.sl > output.asm
echo "Output saved to output.asm"


# 3. Setup Simulator
echo "--- Step 3: Preparing Simulator ---"
cp output.asm 8bit-computer/tests/program.asm

# 4. Run Simulator
echo "--- Step 4: Running Simulation ---"
cd 8bit-computer
./asm/asm.py tests/program.asm > memory.list
make clean && make run
