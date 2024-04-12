#include "RISCV32.h"
#include <cstdint>
#include <iostream>
#include <string>

#include <fstream>

uint32_t RISCV32::reg32[32];
uint8_t RISCV32::Memory32::mem[MEM_SIZE];

RISCV32::RISCV32(
    bool mem_access, bool debug, bool M, bool A, bool F,
    const char* program_file, uint32_t mem_start, uint32_t entrypoint
    ) {
    mem_access_align = mem_access;
    debug_mode = debug;
    ext_M32::extend(M);
    ext_A32::extend(A);
    ext_F32::extend(F);

    // Initialize program
    Memory32::read_program(program_file);

    mem_start_addr = mem_start;
    pc = entrypoint;
    pc_next = pc + 4;
}

void RISCV32::run() {
    
}

void RISCV32::print_inst(uint32_t pc, std::string msg) {
    if (debug_mode == 0) return;
    std::cout << "pc: ";
    std::cout.width(8);
    std::cout.fill('0');
    std::cout << std::hex << pc << ": " << msg << std::endl;
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
    std::cout << "Memory" << std::endl;
    std::cout << "  Address |   Data" << std::endl;
    std::cout << "--------------------" << std::endl;
    for (int i = 0; i < MEM_SIZE / 4; i++) {
        std::cout << " ";
        std::cout.width(8);
        std::cout << std::hex << i << " | ";
        int data = (mem[i * 4 + 3] << 24) | (mem[i * 4 + 2] << 16) | (mem[i * 4 + 1] << 8) | mem[i * 4];
        std::cout.width(8);
        std::cout.fill('0');
        std::cout << std::hex << data << std::endl;
    }
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

// Instructions
RISCV32::base_I32::base_I32() {
    // Nothing to do..!
}

// U-type
void RISCV32::base_I32::lui(uint32_t rd, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "lui " + std::to_string(rd) + ", " + std::to_string(imm));
    }
    if (rd != 0) reg32[rd] = imm;
}

void RISCV32::base_I32::auipc(uint32_t rd, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "auipc " + std::to_string(rd) + ", " + std::to_string((int32_t)imm));
    }
    if (rd != 0) reg32[rd] = pc + (int32_t)imm;
}

// J-type
void RISCV32::base_I32::jal(uint32_t rd, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "jal " + std::to_string(rd) + ", " + std::to_string((int32_t)imm));
    }
    if (rd != 0) reg32[rd] = pc + 4;
    pc_next = pc + (int32_t)imm;
}

// I-type
void RISCV32::base_I32::jalr(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "jalr " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string((int32_t)imm));
    }
    if (rd != 0) reg32[rd] = pc + 4;
    pc_next = (reg32[rs1] + (int32_t)imm) & 0xFFFFFFFE;
}

// B-type
void RISCV32::base_I32::beq(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "beq " + std::to_string(rs1) + ", " + std::to_string(rs2) + ", " + std::to_string((int32_t)imm));
    }
    if (reg32[rs1] == reg32[rs2]) {
        pc_next = pc + (int32_t)imm;
    }
}

void RISCV32::base_I32::bne(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "bne " + std::to_string(rs1) + ", " + std::to_string(rs2) + ", " + std::to_string((int32_t)imm));
    }
    if (reg32[rs1] != reg32[rs2]) {
        pc_next = pc + (int32_t)imm;
    }
}

void RISCV32::base_I32::blt(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "blt " + std::to_string(rs1) + ", " + std::to_string(rs2) + ", " + std::to_string((int32_t)imm));
    }
    if ((int32_t)reg32[rs1] < (int32_t)reg32[rs2]) {
        pc_next = pc + (int32_t)imm;
    }
}

void RISCV32::base_I32::bge(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "bge " + std::to_string(rs1) + ", " + std::to_string(rs2) + ", " + std::to_string((int32_t)imm));
    }
    if ((int32_t)reg32[rs1] >= (int32_t)reg32[rs2]) {
        pc_next = pc + (int32_t)imm;
    }
}

void RISCV32::base_I32::bltu(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "bltu " + std::to_string(rs1) + ", " + std::to_string(rs2) + ", " + std::to_string((int32_t)imm));
    }
    if (reg32[rs1] < reg32[rs2]) {
        pc_next = pc + (int32_t)imm;
    }
}

void RISCV32::base_I32::bgeu(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "bgeu " + std::to_string(rs1) + ", " + std::to_string(rs2) + ", " + std::to_string((int32_t)imm));
    }
    if (reg32[rs1] >= reg32[rs2]) {
        pc_next = pc + (int32_t)imm;
    }
}

// I-type
void RISCV32::base_I32::lb(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "lb " + std::to_string(rd) + ", " + std::to_string((int32_t)imm) + "(" + std::to_string(rs1) + ")");
    }
    uint8_t data;
    Memory32::read_mem_u8(reg32[rs1] + (int32_t)imm, &data);
    if (rd != 0) reg32[rd] = (int32_t)data;
}

void RISCV32::base_I32::lh(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "lh " + std::to_string(rd) + ", " + std::to_string((int32_t)imm) + "(" + std::to_string(rs1) + ")"); 
    }
    uint16_t data;
    Memory32::read_mem_u16(reg32[rs1] + (int32_t)imm, &data);
    if (rd != 0) reg32[rd] = (int32_t)data;
}

void RISCV32::base_I32::lw(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "lw " + std::to_string(rd) + ", " + std::to_string((int32_t)imm) + "(" + std::to_string(rs1) + ")");  
    }
    uint32_t data;
    Memory32::read_mem_u32(reg32[rs1] + (int32_t)imm, &data);
    if (rd != 0) reg32[rd] = (int32_t)data;
}

void RISCV32::base_I32::lbu(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "lbu " + std::to_string(rd) + ", " + std::to_string((int32_t)imm) + "(" + std::to_string(rs1) + ")"); 
    }
    uint8_t data;
    Memory32::read_mem_u8(reg32[rs1] + (int32_t)imm, &data);
    if (rd != 0) reg32[rd] = (uint32_t)data;
}

void RISCV32::base_I32::lhu(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "lhu " + std::to_string(rd) + ", " + std::to_string((int32_t)imm) + "(" + std::to_string(rs1) + ")");
    }
    uint16_t data;
    Memory32::read_mem_u16(reg32[rs1] + (int32_t)imm, &data);
    if (rd != 0) reg32[rd] = (uint32_t)data;
}

// S-type
void RISCV32::base_I32::sb(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "sb " + std::to_string(rs2) + ", " + std::to_string((int32_t)imm) + "(" + std::to_string(rs1) + ")");
    }
    Memory32::write_mem_u8(reg32[rs1] + (int32_t)imm, reg32[rs2] & 0xFF);
}

void RISCV32::base_I32::sh(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "sh " + std::to_string(rs2) + ", " + std::to_string((int32_t)imm) + "(" + std::to_string(rs1) + ")");
    }
    Memory32::write_mem_u16(reg32[rs1] + (int32_t)imm, reg32[rs2] & 0xFFFF);
}

void RISCV32::base_I32::sw(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "sw " + std::to_string(rs2) + ", " + std::to_string((int32_t)imm) + "(" + std::to_string(rs1) + ")");
    }
    Memory32::write_mem_u32(reg32[rs1] + (int32_t)imm, reg32[rs2]);
}

// I-type
void RISCV32::base_I32::addi(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "addi " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(imm));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] + imm;
}

void RISCV32::base_I32::slti(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "slti " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(imm));
    }
    if (rd != 0) reg32[rd] = (int32_t)reg32[rs1] < (int32_t)imm ? 1 : 0;
}

void RISCV32::base_I32::sltiu(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "sltiu " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(imm));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] < imm ? 1 : 0;
}

void RISCV32::base_I32::xori(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "xori " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(imm));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] ^ imm;
}

void RISCV32::base_I32::ori(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "ori " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(imm));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] | imm;
}

void RISCV32::base_I32::andi(uint32_t rd, uint32_t rs1, uint32_t imm) {
    if (debug_mode == 1) {
        print_inst(pc, "andi " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(imm));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] & imm;
}

void RISCV32::base_I32::slli(uint32_t rd, uint32_t rs1, uint32_t shamt) {
    if (debug_mode == 1) {
        print_inst(pc, "slli " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(shamt));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] << (shamt & 0x1F); // Only lower 5-bits matters.
}

void RISCV32::base_I32::srli(uint32_t rd, uint32_t rs1, uint32_t shamt) {
    if (debug_mode == 1) {
        print_inst(pc, "srli " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(shamt));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] >> (shamt & 0x1F); // Only lower 5-bits matters.
}

void RISCV32::base_I32::srai(uint32_t rd, uint32_t rs1, uint32_t shamt) {
    if (debug_mode == 1) {
        print_inst(pc, "srai " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(shamt));
    }
    if (rd != 0) reg32[rd] = (int32_t)reg32[rs1] >> (shamt & 0x1F); // Only lower 5-bits matters.
}

// R-type
void RISCV32::base_I32::add(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "add " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] + reg32[rs2];
}

void RISCV32::base_I32::sub(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "sub " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] - reg32[rs2];
}

void RISCV32::base_I32::sll(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "sll " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] << (reg32[rs2] & 0x1F); // Only lower 5-bits matters.
}

void RISCV32::base_I32::slt(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "slt " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = (int32_t)reg32[rs1] < (int32_t)reg32[rs2] ? 1 : 0;
}

void RISCV32::base_I32::sltu(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "sltu " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] < reg32[rs2] ? 1 : 0;
}

void RISCV32::base_I32::xor_(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "xor " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] ^ reg32[rs2];
}

void RISCV32::base_I32::srl(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "srl " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] >> (reg32[rs2] & 0x1F); // Only lower 5-bits matters.
}

void RISCV32::base_I32::sra(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "sra " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = (int32_t)reg32[rs1] >> (reg32[rs2] & 0x1F); // Only lower 5-bits matters.
}

void RISCV32::base_I32::or_(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "or " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] | reg32[rs2];
}

void RISCV32::base_I32::and_(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(pc, "and " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(rs2));
    }
    if (rd != 0) reg32[rd] = reg32[rs1] & reg32[rs2];
}
