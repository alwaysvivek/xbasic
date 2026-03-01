#!/usr/bin/env python3
"""
8-bit Computer Assembler
Modernized version for Python 3.10+
"""

import re
import sys
import argparse
from typing import Dict, List, Optional, Union

# Instruction Set Definition
INSTRUCTIONS = {
    "nop":  0x00,
    "call": 0b00000001,
    "ret":  0b00000010,
    "lda":  0b10000111,
    "out":  0b00000011,
    "in":   0b00000100,
    "hlt":  0b00000101,
    "cmp":  0b00000110,
    "sta":  0b10111000,
    "jmp":  0b00011000,
    "jz":   0b00011001,
    "jnz":  0b00011010,
    "je":   0b00011001,
    "jne":  0b00011010,
    "jc":   0b00011011,
    "jnc":  0b00011100,
    "push": 0b00100000,
    "pop":  0b00101000,
    "add":  0b01000000,
    "sub":  0b01001000,
    "inc":  0b01010000,
    "dec":  0b01011000,
    "and":  0b01100000,
    "or":   0b01101000,
    "xor":  0b01110000,
    "adc":  0b01111000,
    "ldi":  0b00010000,
    "mov":  0b10000000,
}

# Register Mapping
REGISTERS = {
    "A": 0b000,
    "B": 0b001,
    "C": 0b010,
    "D": 0b011,
    "E": 0b100,
    "F": 0b101,
    "G": 0b110,
    "M": 0b111,  # Memory reference (usually uses address in HL/MAR)
}

MEM_SIZE = 256

class Assembler:
    def __init__(self):
        self.memory = [0] * MEM_SIZE
        self.current_address = 0
        self.labels = {}
        self.data_values = {}
        self.data_addresses = {}
        self.sections = {"TEXT": 0, "DATA": 1}
        self.current_section = self.sections["TEXT"]

    def rich_int(self, value: str) -> int:
        """Parse integers in decimal, hex (0x), or binary (0b) format."""
        try:
            if value.startswith("0x"):
                return int(value, 16)
            elif value.startswith("0b"):
                return int(value, 2)
            else:
                return int(value)
        except ValueError:
            return 0

    def parse_line(self, line: str):
        if not (line := re.sub(r";.*", "", line).strip()): return
        if line.startswith("."):
            self.current_section = self.sections[line[1:].upper()]
        elif self.current_section == self.sections["DATA"]:
            self._handle_data(line)
        else:
            self._handle_text(line)

    def _handle_data(self, line: str):
        if "=" in line:
            name, value = map(str.strip, line.split("=", 1))
            self.data_values[name] = self.rich_int(value)

    def _handle_text(self, line: str):
        tokens = line.split()
        if not tokens: return

        # Handle labels
        if tokens[0].endswith(":"):
            self.labels[tokens[0].rstrip(":")] = self.current_address
            tokens = tokens[1:]
            if not tokens: return

        instruction = tokens[0].lower()
        if instruction not in INSTRUCTIONS:
            self._emit(tokens[0])
            return

        opcode = INSTRUCTIONS[instruction]

        if instruction == "ldi":
            reg_code = REGISTERS[tokens[1].upper()]
            opcode = (opcode & 0b11111000) | reg_code
            self._emit(opcode)
            self._emit(tokens[2])
        elif instruction in ("push", "pop"):
            reg_code = REGISTERS[tokens[1].upper()]
            opcode = (opcode & 0b11111000) | reg_code
            self._emit(opcode)
        elif instruction == "mov":
            dest_code = REGISTERS[tokens[1].upper()]
            src_code = REGISTERS[tokens[2].upper()]
            opcode = (opcode & 0b11111000) | src_code
            opcode = (opcode & 0b11000111) | (dest_code << 3)
            self._emit(opcode)
        else:
            self._emit(opcode)
            # Only emit extra byte for instructions that take an 8-bit immediate/address
            if instruction in ("lda", "sta", "jmp", "jz", "jnz", "je", "jne", "jc", "jnc", "call", "out", "in"):
                if len(tokens) > 1:
                    self._emit(tokens[1])

    def _emit(self, value: Union[int, str]):
        if self.current_address >= MEM_SIZE:
            raise MemoryError("Program exceeds memory size")
        self.memory[self.current_address] = value
        self.current_address += 1

    def assemble(self, filename: str):
        with open(filename, 'r') as f:
            for line in f:
                self.parse_line(line)

        # Write data into memory at the end of the program
        for name, value in self.data_values.items():
            self.data_addresses[name] = self.current_address
            self.memory[self.current_address] = value
            self.current_address += 1

        # Merge labels and data addresses
        self.data_addresses.update(self.labels)

        # Final pass: resolve labels/variables
        final_bytes = []
        for val in self.memory[:self.current_address]:
            if isinstance(val, str):
                if val.startswith("%"):
                    # Variable reference
                    var_name = val.lstrip("%")
                    final_bytes.append(self.data_addresses.get(var_name, 0))
                else:
                    # Immediate value or label
                    final_bytes.append(self.data_addresses.get(val, self.rich_int(val)))
            else:
                final_bytes.append(val)

        return final_bytes

def main():
    parser = argparse.ArgumentParser(description="8-bit Computer Assembler")
    parser.add_argument("file", help="Input assembly file")
    args = parser.parse_args()

    assembler = Assembler()
    try:
        program = assembler.assemble(args.file)
        print(' '.join([f'{b:02x}' for b in program]))
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
