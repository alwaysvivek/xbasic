import sys
import os
import subprocess
import argparse

def get_resource_path(relative_path):
    # This works when running from the source tree
    return os.path.join(os.path.dirname(__file__), relative_path)

def main():
    parser = argparse.ArgumentParser(description="XBasic-Modern Toolchain")
    parser.add_argument("input", help="XBasic-Modern source file (.sl)")
    parser.add_argument("--debug", action="store_true", help="Show full simulation logs and internal states")
    args = parser.parse_args()

    if not os.path.exists(args.input):
        print(f"\033[91mError:\033[0m {args.input} not found.")
        sys.exit(1)

    pkg_root = os.path.dirname(__file__)
    compiler_bin = os.path.join(os.getcwd(), "compiler") 
    sim_dir = get_resource_path("simulator")
    asm_py = os.path.join(sim_dir, "asm", "asm.py")

    # 1. Compile
    if args.debug: print(f"--- Compiling {args.input} ---")
    try:
        subprocess.check_call([compiler_bin, args.input], stdout=open("output.asm", "w"), stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        print("\033[91mCompilation Failed\033[0m")
        sys.exit(1)

    # 2. Assemble
    if args.debug: print("--- Assembling ---")
    with open("memory.list", "w") as f:
        subprocess.check_call([sys.executable, asm_py, "output.asm"], stdout=f)

    # 3. Simulate
    if args.debug: print("--- Simulating ---")
    rtl_files = [
        "alu.v", "cpu_control.v", "cpu_registers.v", "cpu.v", "machine.v", "parameters.v",
        "library/clock.v", "library/counter.v", "library/ram.v", "library/register.v", "library/tristate_buffer.v",
        "tb/machine_tb.v"
    ]
    rtl_paths = [os.path.join(sim_dir, "rtl", f) for f in rtl_files]
    
    cmd = ["iverilog", "-o", "computer", "-Wall", f"-I{sim_dir}"] + rtl_paths
    
    if args.debug:
        subprocess.check_call(cmd)
        subprocess.check_call(["vvp", "-n", "computer"])
    else:
        # User-friendly mode: Filter output
        subprocess.check_call(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        result = subprocess.run(["vvp", "-n", "computer"], capture_output=True, text=True)
        
        # Parse output for PRINT statements
        for line in result.stdout.splitlines():
            if line.startswith("Output:"):
                # Clean up "Output: 15 ($0f)" -> "15"
                val = line.split(":")[1].split("(")[0].strip()
                print(f"\033[92m>\033[0m {val}")
            elif "halted" in line.lower() and args.debug:
                print(line)

if __name__ == "__main__":
    main()
