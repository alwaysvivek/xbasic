INPUT_FILE=${1:-tests/input.sl}

# 1. Build the compiler
echo "--- Step 1: Building Compiler ---"
make clean && make

# 2. Compile XBasic to Assembly
echo "--- Step 2: Compiling $INPUT_FILE ---"
mkdir -p tests
./compiler "$INPUT_FILE" > output.asm
echo "Output saved to output.asm"

# 3. Setup Simulator
echo "--- Step 3: Preparing Simulator ---"
cp output.asm 8bit-computer-modern/tests/program.asm

# 4. Run Simulator
echo "--- Step 4: Running Simulation ---"
cd 8bit-computer-modern
./asm/asm.py tests/program.asm > memory.list
make clean && make run
