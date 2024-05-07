gcc := riscv64-unknown-elf-gcc
objcopy := riscv64-unknown-elf-objcopy
dump := riscv64-unknown-elf-objdump

CC := g++

SRCs := $(wildcard ./src/*.c)

.PHONY: all
all:
	@echo "Usage"
	@echo ""
	@echo "Build 32-bit RISC-V Emulator"
	@echo "make RISCV32"
	@echo ""
	@echo "Build 64-bit RISC-V Emulator"
	@echo "make RISCV64"

.PHONY: RISCV32
RISCV32: riscv32_emulator.out out_binary.bin

RISCV64: riscv64_emulator.out out_binary.bin

riscv32_emulator.out: main.cpp RISCV32.cpp
	@echo "Emulator Building"
	$(CC) -std=c++11 -o $@ $^

riscv64_emulator.out: 
	@echo "RV64I Not Supported yet.."

out_binary: $(SRCs)
	$(gcc) -Wl,-Ttext=0x0 -nostdlib -march=rv32i -mabi=ilp32 -o $@ $^

out_binary.bin: out_binary
	$(objcopy) -O binary $^ $@


.PHONY: clean
clean:
	@echo "Clean all"
	rm -rf *.out *.bin
	rm out_binary
# gcc -o $@ $^
