#include "RISCV32.h"
#include <cstdint>
#include <iostream>
#include <string>

#include <fstream>

// Global Variables
int RISCV32::mem_access_align;
int RISCV32::debug_mode;
uint32_t RISCV32::pc;
uint32_t RISCV32::pc_next;
uint32_t RISCV32::reg32[32];
uint8_t RISCV32::Memory32::mem[MEM_SIZE]; // Initialized all to 0

// bool RISCV32::ext_M32::extended;
// bool RISCV32::ext_A32::extended;
// bool RISCV32::ext_F32::extended;

RISCV32::RISCV32(
    bool mem_access, bool debug, bool M, bool A, bool F,
    const char* program_file, uint32_t mem_start, uint32_t entrypoint
    ) {
    mem_access_align = mem_access;
    // debug_mode = debug;
    debug_mode = true;
    // ext_M32::extend(M);
    // ext_A32::extend(A);
    // ext_F32::extend(F);

    // Initialize program
    Memory32::read_program(program_file);
    
    // Status
    running = false;

    // mem_start_addr = mem_start;
    pc = entrypoint;
    pc_next = pc;
}

void RISCV32::run() {
    running = true;
    reg32[0] = 0;
    reg32[1] = END; // to end the program when main
    reg32[2] = MEM_SIZE - 1; // stack pointer at the largest address

    while (running && pc_next != (END & 0xFFFFFFFE) ) {
        pc = pc_next;
        pc_next = pc + 4;
        uint32_t instr;
        Memory32::read_mem_u32(pc, &instr);

        if (instr == 0x00000000) { // noop
            running = false;
            break;
        } 
        execute32(instr);
    }

    std::cout << "Program Ends." << std::endl;
    RISCV32::print_reg_all();
    RISCV32::Memory32::print_mem_all();
}

void RISCV32::print_inst(uint32_t pc, std::string msg) {
    if (RISCV32::debug_mode == 0) return;
    std::cout << "pc: ";
    std::cout.width(8);
    std::cout.fill('0');
    std::cout << std::hex << pc << ": " << msg << std::endl;
}

void RISCV32::print_reg_all() {
    std::ofstream output_reg;
    output_reg.open("./output/register.txt");

    output_reg << "Register" << std::endl;
    output_reg << "--------------------" << std::endl;
    for (int i = 0; i < 32; i++) {
        output_reg << "x[";
        output_reg.width(2);
        output_reg << std::hex << i << "] = ";
        output_reg.width(8);
        output_reg.fill('0');
        output_reg << std::hex << RISCV32::reg32[i] << std::endl;
    }
    output_reg.close();
}

uint32_t RISCV32::imm_gen(uint32_t instr) {
    int32_t instr_s = (int32_t)instr;

    uint32_t opcode = instr & 0x7F;
    uint32_t funct3 = (instr >> 12) & 0x7;
    switch (opcode) {
        case 0x37: {
            return instr & 0xFFFFF000;
        } break;
        case 0x17: {
            return instr & 0xFFFFF000;
        } break;
        case 0x6F: {
            return ((instr_s >> 11) & 0xFFF00000) | instr & 0x000FF000 | ((instr >> 9) & 0x00000800) | ((instr >> 20) & 0x000007FE);
        } break;
        case 0x67: {
            return ((instr_s >> 20) & 0xFFFFF000) | ((instr >> 20) & 0x00000FFF);
        } break;
        case 0x63: {
            return ((instr_s >> 19) & 0xFFFFF000) | ((instr << 4) & 0x00000800) | ((instr >> 20) & 0x000007E0) | ((instr >> 7) & 0x0000001E); 
        } break;
        case 0x03: {
            return ((instr_s >> 20) & 0xFFFFF000) | ((instr >> 20) & 0x00000FFF);
        } break;
        case 0x23: {
            return ((instr_s >> 20) & 0xFFFFF000) | ((instr >> 20) & 0x00000FE0) | ((instr >> 7) & 0x000001F);
        } break;
        case 0x13: {
            switch (funct3) {
                case 0x1:
                case 0x5: {
                    return (instr >> 20) & 0x1F;
                } break;
                case 0x0:
                case 0x2:
                case 0x3:
                case 0x4:
                case 0x6:
                case 0x7: {
                    return ((instr_s >> 20) & 0xFFFFF000) | ((instr >> 20) & 0x00000FFF);
                }
                default: { return -1; } break;
            }
        } break;
        case 0x33: {
            return -1;
        }
        default: {return -1;} break;
    }
}
void RISCV32::execute32(uint32_t instr) {
    uint32_t opcode = instr & 0x7F;
    uint32_t rd = (instr >> 7) & 0x1F;
    uint32_t rs1 = (instr >> 15) & 0x1F;
    uint32_t rs2 = (instr >> 20) & 0x1F;
    uint32_t funct3 = (instr >> 12) & 0x7;
    uint32_t funct7 = (instr >> 25) & 0x7F;

    switch (opcode) {
        case 0x37: {
            base_I32::lui(rd, imm_gen(instr));
        } break;

        case 0x17: {
            base_I32::auipc(rd, imm_gen(instr));
        } break;

        case 0x6F: {
            base_I32::jal(rd, imm_gen(instr));
        } break;

        case 0x67: {
            base_I32::jalr(rd, rs1, imm_gen(instr));
        } break;

        case 0x63: {
            switch (funct3) {
                case 0x0: {
                    base_I32::beq(rs1, rs2, imm_gen(instr));
                } break;
                case 0x1: {
                    base_I32::bne(rs1, rs2, imm_gen(instr));
                } break;
                case 0x4: {
                    base_I32::blt(rs1, rs2, imm_gen(instr));
                } break;
                case 0x5: {
                    base_I32::bge(rs1, rs2, imm_gen(instr));
                } break;
                case 0x6: {
                    base_I32::bltu(rs1, rs2, imm_gen(instr));
                } break;
                case 0x7: {
                    base_I32::bgeu(rs1, rs2, imm_gen(instr));
                } break;
                default: {
                    INSTR_ERR;
                } break;
            }
        } break;

        case 0x03: {
            switch (funct3) {
                case 0x0: {
                    base_I32::lb(rd, rs1, imm_gen(instr));
                } break;
                case 0x1: {
                    base_I32::lh(rd, rs1, imm_gen(instr));
                } break;
                case 0x2: {
                    base_I32::lw(rd, rs1, imm_gen(instr));
                } break;
                case 0x4: {
                    base_I32::lbu(rd, rs1, imm_gen(instr));
                } break;
                case 0x5: {
                    base_I32::lhu(rd, rs1, imm_gen(instr));
                } break;
                default: {
                    INSTR_ERR;
                } break;
            }
        } break;

        case 0x23: {
            switch (funct3) {
                case 0x0: {
                    base_I32::sb(rs1, rs2, imm_gen(instr));
                } break;
                case 0x1: {
                    base_I32::sh(rs1, rs2, imm_gen(instr));
                } break;
                case 0x2: {
                    base_I32::sw(rs1, rs2, imm_gen(instr));
                } break;
                default: {
                    INSTR_ERR;
                } break;
            }
        } break;

        case 0x13: {
            switch (funct3) {
                case 0x0: {
                    base_I32::addi(rd, rs1, imm_gen(instr));
                } break;
                case 0x2: {
                    base_I32::slti(rd, rs1, imm_gen(instr));
                } break;
                case 0x3: {
                    base_I32::sltiu(rd, rs1, imm_gen(instr));
                } break;
                case 0x4: {
                    base_I32::xori(rd, rs1, imm_gen(instr));
                } break;
                case 0x6: {
                    base_I32::ori(rd, rs1, imm_gen(instr));
                } break;
                case 0x7: {
                    base_I32::andi(rd, rs1, imm_gen(instr));
                } break;
                case 0x1: {
                    base_I32::slli(rd, rs1, imm_gen(instr));
                } break;
                case 0x5: {
                    switch (funct7) {
                        case 0x00: {
                            base_I32::srli(rd, rs1, imm_gen(instr));
                        } break;
                        case 0x20: {
                            base_I32::srai(rd, rs1, imm_gen(instr));
                        } break;
                        default: {
                            INSTR_ERR;
                        } break;
                    }
                } break;
                default: {
                    INSTR_ERR;
                } break;
            }
        } break;
        
        case 0x33: {
            switch (funct3) {
                case 0x0: {
                    switch (funct7) {
                        case 0x00: {
                            base_I32::add(rd, rs1, rs2);
                        } break;
                        case 0x20: {
                            base_I32::sub(rd, rs1, rs2);
                        } break;
                        default: {
                            INSTR_ERR;
                        } break;
                    }
                } break;
                case 0x1: {
                    base_I32::sll(rd, rs1, rs2);
                } break;
                case 0x2: {
                    base_I32::slt(rd, rs1, rs2);
                } break;
                case 0x3: {
                    base_I32::sltu(rd, rs1, rs2);
                } break;
                case 0x4: {
                    base_I32::xor_(rd, rs1, rs2);
                } break;
                case 0x5: {
                    switch (funct7) {
                        case 0x00: {
                            base_I32::srl(rd, rs1, rs2);
                        } break;
                        case 0x20: {
                            base_I32::sra(rd, rs1, rs2);
                        } break;
                        default: {
                            INSTR_ERR;
                        } break;
                    }
                } break;
                case 0x6: {
                    base_I32::or_(rd, rs1, rs2);
                } break;
                case 0x7: {
                    base_I32::and_(rd, rs1, rs2);
                } break;
                default: {
                    INSTR_ERR;
                } break;
            }
        } break;
        default: break;
    }
}

void RISCV32::Memory32::read_mem_u8(uint32_t addr, uint8_t* data) {
    if (mem_access_align == 1 && addr % 1 != 0) {
        MEM_ALIGN_ERR;
    }
    if (addr >= MEM_SIZE) {
        MEM_OUT_ERR;
    }
    *data = mem[addr];
}

void RISCV32::Memory32::read_mem_u16(uint32_t addr, uint16_t* data) {
    if (mem_access_align == 1 && addr % 2 != 0) {
        MEM_ALIGN_ERR;
    }
    if (addr >= MEM_SIZE - 1) {
        MEM_OUT_ERR;
    }
    *data = (mem[addr + 1] << 8) | mem[addr];
}

void RISCV32::Memory32::read_mem_u32(uint32_t addr, uint32_t* data) {
    if (mem_access_align == 1 && addr % 4 != 0) {
        MEM_ALIGN_ERR;
    }
    if (addr >= MEM_SIZE - 3) {
        MEM_OUT_ERR;
    }
    *data = (mem[addr + 3] << 24) | (mem[addr + 2] << 16) | (mem[addr + 1] << 8) | mem[addr];    
}

void RISCV32::Memory32::write_mem_u8(uint32_t addr, uint8_t data) {
    if (mem_access_align == 1 && addr % 1 != 0) {
        MEM_ALIGN_ERR;
    }
    if (addr >= MEM_SIZE) {
        MEM_OUT_ERR;
    }
    mem[addr] = data;
}

void RISCV32::Memory32::write_mem_u16(uint32_t addr, uint16_t data) {
    if (mem_access_align == 1 && addr % 2 != 0) {
        MEM_ALIGN_ERR;
    }
    if (addr >= MEM_SIZE - 1) {
        MEM_OUT_ERR;
    }
    mem[addr] = data & 0xFF;
    mem[addr + 1] = (data >> 8) & 0xFF;
}

void RISCV32::Memory32::write_mem_u32(uint32_t addr, uint32_t data) {
    if (mem_access_align == 1 && addr % 4 != 0) {
        MEM_ALIGN_ERR;
    }
    if (addr >= MEM_SIZE - 3) {
        MEM_OUT_ERR;
    }
    mem[addr] = data & 0xFF;
    mem[addr + 1] = (data >> 8) & 0xFF;
    mem[addr + 2] = (data >> 16) & 0xFF;
    mem[addr + 3] = (data >> 24) & 0xFF;
}

void RISCV32::Memory32::read_program(const char* program_file) { 
    std::ifstream program(program_file, std::ios::in | std::ios::binary);
    if (!program.is_open()) {
        throw std::runtime_error("Failed to open program file.");
    }
    program.read((char*)Memory32::mem, MEM_SIZE);
    program.close();
}

void RISCV32::Memory32::print_mem_all() {
    std::ofstream output_mem;
    output_mem.open("./output/memory.txt");

    output_mem << "Memory" << std::endl;
    output_mem << "  Address |   Data" << std::endl;
    output_mem << "--------------------" << std::endl;
    for (int i = 0; i < MEM_SIZE / 4; i++) {
        output_mem << " ";
        output_mem.width(8);
        output_mem << std::hex << (i << 2) << " | ";
        int data = (mem[i * 4 + 3] << 24) | (mem[i * 4 + 2] << 16) | (mem[i * 4 + 1] << 8) | mem[i * 4];
        output_mem.width(8);
        output_mem.fill('0');
        output_mem << std::hex << data << std::endl;
    }
    output_mem.close();
}

void RISCV32::Memory32::print_mem_u8(uint32_t addr) {
    uint8_t data;
    read_mem_u8(addr, &data);
    std::cout << "Memory[" << addr << "]: " << std::hex << (int)data << std::endl;
}

void RISCV32::Memory32::print_mem_u16(uint32_t addr) {
    uint16_t data;
    read_mem_u16(addr, &data);
    std::cout << "Memory[" << addr << "]: " << std::hex << (int)data << std::endl;
}

void RISCV32::Memory32::print_mem_u32(uint32_t addr) {
    uint32_t data;
    read_mem_u32(addr, &data);
    std::cout << "Memory[" << addr << "]: " << std::hex << (int)data << std::endl;
}

// U-type
void RISCV32::base_I32::lui(uint32_t rd, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "lui x" + std::to_string(rd) + ", " + std::to_string((int32_t)imm));
    }
    if (rd != 0) reg32[rd] = imm;
}

void RISCV32::base_I32::auipc(uint32_t rd, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "auipc x" + std::to_string(rd) + ", " + std::to_string((int32_t)imm));
    }
    if (rd != 0) reg32[rd] = pc + (int32_t)imm;
}

// J-type
void RISCV32::base_I32::jal(uint32_t rd, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "jal x" + std::to_string(rd) + ", " + std::to_string((int32_t)imm));
    }
    if (rd != 0) reg32[rd] = pc + 4;
    pc_next = pc + (int32_t)imm;
}

// I-type
void RISCV32::base_I32::jalr(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "jalr x" + std::to_string(rd) + ", " + std::to_string((int32_t)imm) + "(x" + std::to_string(rs1) + ")");
    }
    if (rd != 0) reg32[rd] = pc + 4;
    pc_next = (reg32[rs1] + (int32_t)imm) & 0xFFFFFFFE;
}

// B-type
void RISCV32::base_I32::beq(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "beq x" + std::to_string(rs1) + ", x" + std::to_string(rs2) + ", " + std::to_string((int32_t)imm));
    }
    if (reg32[rs1] == reg32[rs2]) {
        pc_next = pc + (int32_t)imm;
    }
}

void RISCV32::base_I32::bne(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "bne x" + std::to_string(rs1) + ", x" + std::to_string(rs2) + ", " + std::to_string((int32_t)imm));
    }
    if (reg32[rs1] != reg32[rs2]) {
        pc_next = pc + (int32_t)imm;
    }
}

void RISCV32::base_I32::blt(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "blt x" + std::to_string(rs1) + ", x" + std::to_string(rs2) + ", " + std::to_string((int32_t)imm));
    }
    if ((int32_t)reg32[rs1] < (int32_t)reg32[rs2]) {
        pc_next = pc + (int32_t)imm;
    }
}

void RISCV32::base_I32::bge(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "bge x" + std::to_string(rs1) + ", x" + std::to_string(rs2) + ", " + std::to_string((int32_t)imm));
    }
    if ((int32_t)reg32[rs1] >= (int32_t)reg32[rs2]) {
        pc_next = pc + (int32_t)imm;
    }
}

void RISCV32::base_I32::bltu(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "bltu x" + std::to_string(rs1) + ", x" + std::to_string(rs2) + ", " + std::to_string((int32_t)imm));
    }
    if (reg32[rs1] < reg32[rs2]) {
        pc_next = pc + (int32_t)imm;
    }
}

void RISCV32::base_I32::bgeu(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "bgeu x" + std::to_string(rs1) + ", x" + std::to_string(rs2) + ", " + std::to_string((int32_t)imm));
    }
    if (reg32[rs1] >= reg32[rs2]) {
        pc_next = pc + (int32_t)imm;
    }
}

// I-type
void RISCV32::base_I32::lb(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "lb x" + std::to_string(rd) + ", " + std::to_string((int32_t)imm) + "(x" + std::to_string(rs1) + ")");
    }
    uint8_t data;
    Memory32::read_mem_u8(reg32[rs1] + (int32_t)imm, &data);
    if (rd != 0) reg32[rd] = (int32_t)data;
}

void RISCV32::base_I32::lh(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "lh x" + std::to_string(rd) + ", " + std::to_string((int32_t)imm) + "(x" + std::to_string(rs1) + ")"); 
    }
    uint16_t data;
    Memory32::read_mem_u16(reg32[rs1] + (int32_t)imm, &data);
    if (rd != 0) reg32[rd] = (int32_t)data;
}

void RISCV32::base_I32::lw(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "lw x" + std::to_string(rd) + ", " + std::to_string((int32_t)imm) + "(x" + std::to_string(rs1) + ")");  
    }
    uint32_t data;
    Memory32::read_mem_u32(reg32[rs1] + (int32_t)imm, &data);
    if (rd != 0) reg32[rd] = (int32_t)data;
}

void RISCV32::base_I32::lbu(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "lbu x" + std::to_string(rd) + ", " + std::to_string((int32_t)imm) + "(x" + std::to_string(rs1) + ")"); 
    }
    uint8_t data;
    Memory32::read_mem_u8(reg32[rs1] + (int32_t)imm, &data);
    if (rd != 0) reg32[rd] = (uint32_t)data;
}

void RISCV32::base_I32::lhu(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "lhu x" + std::to_string(rd) + ", " + std::to_string((int32_t)imm) + "(x" + std::to_string(rs1) + ")");
    }
    uint16_t data;
    Memory32::read_mem_u16(reg32[rs1] + (int32_t)imm, &data);
    if (rd != 0) reg32[rd] = (uint32_t)data;
}

// S-type
void RISCV32::base_I32::sb(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "sb x" + std::to_string(rs2) + ", " + std::to_string((int32_t)imm) + "(x" + std::to_string(rs1) + ")");
    }
    Memory32::write_mem_u8(reg32[rs1] + (int32_t)imm, reg32[rs2] & 0xFF);
}

void RISCV32::base_I32::sh(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "sh x" + std::to_string(rs2) + ", " + std::to_string((int32_t)imm) + "(x" + std::to_string(rs1) + ")");
    }
    Memory32::write_mem_u16(reg32[rs1] + (int32_t)imm, reg32[rs2] & 0xFFFF);
}

void RISCV32::base_I32::sw(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "sw x" + std::to_string(rs2) + ", " + std::to_string((int32_t)imm) + "(x" + std::to_string(rs1) + ")");
    }
    Memory32::write_mem_u32(reg32[rs1] + (int32_t)imm, reg32[rs2]);
}

// I-type
void RISCV32::base_I32::addi(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "addi x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string((int32_t)imm));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] + imm;
}

void RISCV32::base_I32::slti(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "slti x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string((int32_t)imm));
    }
    if (rd != 0) reg32[rd] = (int32_t)reg32[rs1] < (int32_t)imm ? 1 : 0;
}

void RISCV32::base_I32::sltiu(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "sltiu x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string((int32_t)imm));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] < imm ? 1 : 0;
}

void RISCV32::base_I32::xori(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "xori x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string((int32_t)imm));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] ^ imm;
}

void RISCV32::base_I32::ori(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "ori x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string((int32_t)imm));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] | imm;
}

void RISCV32::base_I32::andi(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "andi x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string((int32_t)imm));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] & imm;
}

void RISCV32::base_I32::slli(uint32_t rd, uint32_t rs1, uint32_t shamt) {
    if (debug_mode == 1) {
        print_inst(pc, "slli x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string((int32_t)shamt));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] << (shamt & 0x1F); // Only lower 5-bits matters.
}

void RISCV32::base_I32::srli(uint32_t rd, uint32_t rs1, uint32_t shamt) {
    if (debug_mode == 1) {
        print_inst(pc, "srli x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string((int32_t)shamt));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] >> (shamt & 0x1F); // Only lower 5-bits matters.
}

void RISCV32::base_I32::srai(uint32_t rd, uint32_t rs1, uint32_t shamt) {
    if (debug_mode == 1) {
        print_inst(pc, "srai x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string((int32_t)shamt));
    }
    if (rd != 0) reg32[rd] = (int32_t)reg32[rs1] >> (shamt & 0x1F); // Only lower 5-bits matters.
}

// R-type
void RISCV32::base_I32::add(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "add x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] + reg32[rs2];
}

void RISCV32::base_I32::sub(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "sub x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] - reg32[rs2];
}

void RISCV32::base_I32::sll(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "sll x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] << (reg32[rs2] & 0x1F); // Only lower 5-bits matters.
}

void RISCV32::base_I32::slt(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "slt x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = (int32_t)reg32[rs1] < (int32_t)reg32[rs2] ? 1 : 0;
}

void RISCV32::base_I32::sltu(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "sltu x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] < reg32[rs2] ? 1 : 0;
}

void RISCV32::base_I32::xor_(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "xor x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] ^ reg32[rs2];
}

void RISCV32::base_I32::srl(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "srl x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] >> (reg32[rs2] & 0x1F); // Only lower 5-bits matters.
}

void RISCV32::base_I32::sra(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "sra x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = (int32_t)reg32[rs1] >> (reg32[rs2] & 0x1F); // Only lower 5-bits matters.
}

void RISCV32::base_I32::or_(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "or x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] | reg32[rs2];
}

void RISCV32::base_I32::and_(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "and x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] & reg32[rs2];
}
