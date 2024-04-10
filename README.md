# RISC-V-Emulator
RISC-V Emulator based on C++.

# Usage
```shell
$ make
$ ./riscv_emulator <elf_file>
```

# Architecture
- RV32I
- M, A, F is optional

- The extensions should be determined on compile time.

# TODO
- [o] RISC-V Init
- [x] RV32I Base Instruction Set
- [x] RV32M Standard Extension (Integer Multiplication and Division)
- [x] RV32A Standard Extension (Atomic Instructions)
- [x] RV32F Standard Extension (Single-Precision Floating-Point)
- [x] RV64I Base Instruction Set
- [x] RV64M Standard Extension (Integer Multiplication and Division)
- [x] RV64A Standard Extension (Atomic Instructions)
- [x] RV64F Standard Extension (Single-Precision Floating-Point)
- [x] RV64D Standard Extension (Double-Precision Floating-Point)

