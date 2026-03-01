COMPUTER    = $(wildcard rtl/*.v)
LIBRARIES   = $(wildcard rtl/library/*.v)
ASM         = ./asm/asm.py

build:
	iverilog -o computer -Wall \
		$(COMPUTER) \
		$(LIBRARIES) \
		rtl/tb/machine_tb.v

run: build
	vvp -n computer

clean:
	rm -f computer
	rm -f machine.vcd

view:
	gtkwave machine.vcd gtkwave/config.gtkw

tests:
	bats tests/tests.bats

.PHONY: build run clean view tests
