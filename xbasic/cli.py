import sys
import os
import subprocess
import shutil
import argparse

def get_resource_path(relative_path):
    return os.path.join(os.path.dirname(__file__), relative_path)

def check_dependency(name, install_hint):
    """Check if a system dependency is available, exit with a helpful message if not."""
    if shutil.which(name) is None:
        print(f"\033[91mError:\033[0m '{name}' is not installed.")
        print(f"\033[93mInstall it with:\033[0m {install_hint}")
        print()
        print("Or use Docker for zero-setup:")
        print("  docker run --rm -v $(pwd):/code xbasic /code/your_program.sl")
        sys.exit(1)

def find_compiler():
    """Find the pre-compiled compiler binary, or build it if source is available."""
    # 1. Check for pre-built binary shipped with the package
    pkg_compiler = get_resource_path("compiler")
    if os.path.isfile(pkg_compiler) and os.access(pkg_compiler, os.X_OK):
        return pkg_compiler

    # 2. Check for binary in the current working directory (dev mode)
    cwd_compiler = os.path.join(os.getcwd(), "compiler")
    if os.path.isfile(cwd_compiler) and os.access(cwd_compiler, os.X_OK):
        return cwd_compiler

    # 3. Try to build from source
    makefile = os.path.join(os.path.dirname(get_resource_path(".")), "Makefile")
    if os.path.exists(makefile):
        check_dependency("g++", "brew install gcc (macOS) | apt install g++ (Ubuntu)")
        check_dependency("make", "brew install make (macOS) | apt install make (Ubuntu)")
        try:
            subprocess.check_call(["make", "-C", os.path.dirname(makefile)],
                                  stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            if os.path.isfile(cwd_compiler):
                return cwd_compiler
        except subprocess.CalledProcessError:
            pass

    print("\033[91mError:\033[0m Could not find the XBasic compiler binary.")
    print("Try rebuilding with: make")
    sys.exit(1)

def main():
    parser = argparse.ArgumentParser(
        prog="xbasic",
        description="XBasic — compile and run XBasic programs on an 8-bit CPU",
        epilog="Example: xbasic examples/hello.sl"
    )
    parser.add_argument("input", help="XBasic source file (.sl)")
    parser.add_argument("--debug", action="store_true", help="Show full simulation logs and CPU state")
    args = parser.parse_args()

    if not os.path.exists(args.input):
        print(f"\033[91mError:\033[0m File '{args.input}' not found.")
        sys.exit(1)

    # Check dependencies
    check_dependency("iverilog", "brew install icarus-verilog (macOS) | apt install iverilog (Ubuntu)")
    check_dependency("vvp", "brew install icarus-verilog (macOS) | apt install iverilog (Ubuntu)")

    compiler_bin = find_compiler()
    sim_dir = get_resource_path("simulator")
    asm_py = os.path.join(sim_dir, "asm", "asm.py")

    # 1. Compile
    if args.debug: print(f"--- Compiling {args.input} ---")
    try:
        subprocess.check_call([compiler_bin, args.input], stdout=open("output.asm", "w"), stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError:
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
                val = line.split(":")[1].split("(")[0].strip()
                print(f"\033[92m>\033[0m {val}")
            elif "halted" in line.lower() and args.debug:
                print(line)

if __name__ == "__main__":
    main()
