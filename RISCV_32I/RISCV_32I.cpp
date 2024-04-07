#include "RISCV_32I.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

RISCV_32I::RISCV_32I(const char* filename, uint32_t mem_start, uint32_t entrypoint) {
    std::ifstream program_file(filename, std::ios::in | std::ios::binary);
    if (!program_file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    program_file.read((char*)mem, MEM_SIZE);
    program_file.close();

    mem_start_addr = mem_start;
    pc = entrypoint;
}

void RISCV_32I::inst_execute(uint32_t instr) {
    uint32_t opcode = instr & 0x7F;
    uint32_t rd = (instr >> 7) & 0x1F;
    uint32_t funct3 = (instr >> 12) & 0x7;
    uint32_t rs1 = (instr >> 15) & 0x1F;
    uint32_t rs2 = (instr >> 20) & 0x1F;
    uint32_t funct7 = (instr >> 25) & 0x7F;
    int32_t imm; // vary by the type of the instructions

    switch (opcode) {
        case 0x37: { // LUI
            imm = (instr & 0xFFFFF000);
            break;
        }
        case 0x17: { // AUIPC
            imm = (instr & 0xFFFFF000);
            break;
        }
        case 0x6F: { // JAL
            imm = ((instr >> 11) & 0xFFE00000) | ((instr >> 11) & 0x00100000) | (instr & 0x000FF000) | ((instr >> 9) * 0x00000800) | ((instr >> 20) & 0x000007FE);
            break;
        }
        case 0x67: { // JALR
                     // funct3 = 0x0
            imm = (instr >> 20);
            break;
        }
        case 0x63: { // BRANCH
                    // BEQ, BNE, BLT, BGE, BLTU, BGEU
            imm = ((instr >> 20) & 0xFFFFF000) | ((instr << 4) & 0x00000800) | ((instr >> 20) & 0x000007E0) | ((instr >> 7) & 0x0000001E);
            break;
        }
        case 0x03: { // LOAD
                     // LB, LH, LW, LBU, LHU
            imm = (instr >> 20);
            break;
        }
        case 0x23: { // STORE
                     // SB, SH, SW
            imm = ((instr >> 20) & 0xFFFFF000) | ((instr >> 20) & 0xFE0) | ((instr >> 7) & 0x1F);
            break;
        }
        case 0x13: { // IMMEDIATE
                     // ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAIi
            uint32_t result;
            switch (funct3) {
                case 0x0: // ADDI
                    imm = (instr >> 20);
                    result = (int32_t)reg[rs1] + (int32_t)imm;
                    print_inst(pc, "addi x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string(imm));
                    break;
                case 0x2: // SLTI 
                    imm = (instr >> 20);
                    result = ((int32_t)reg[rs1] < (int32_t)imm) ? 1 : 0;
                    print_inst(pc, "slti x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string(imm));
                    break;
                case 0x3: // SLTIU
                    imm = (instr >> 20);
                    result = ((uint32_t)reg[rs1] < (uint32_t)imm) ? 1 : 0;
                    print_inst(pc, "sltiu x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string(imm));
                    break;
                case 0x4: // XORI
                    imm = (instr >> 20);
                    result = (uint32_t)reg[rs1] ^ (uint32_t)imm;
                    print_inst(pc, "xori x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string(imm));
                    break;
                case 0x6: // ORI
                    imm = (instr >> 20);
                    result = (uint32_t)reg[rs1] | (uint32_t)imm;
                    print_inst(pc, "ori x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string(imm));
                    break;
                case 0x7: // ANDI
                    imm = (instr >> 20);
                    result = (uint32_t)reg[rs1] & (uint32_t)imm;
                    print_inst(pc, "andi x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string(imm));
                    break;
                case 0x1: // SLLI
                    if ((instr >> 25) & 0x7F) {
                        // todo: illegal instruction
                        return;
                    }
                    imm = (instr >> 20) & 0x1F;
                    result = (uint32_t)reg[rs1] << (uint32_t)imm;
                    print_inst(pc, "slli x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string(imm));
                    break;
                case 0x5: // SRLI, SRAI
                    imm = (instr >> 20) & 0x1F;
                    switch (funct7) {
                        case 0x00: // SRLI
                            if ((instr >> 25) & 0x7F) {
                                return;
                            }
                            result = (uint32_t)reg[rs1] >> (uint32_t)imm;
                            print_inst(pc, "srli x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string(imm));
                            break;
                        case 0x20: // SRAI
                            if ((instr >> 25) & 0x5F) {
                                return;
                            }
                            result = (int32_t)reg[rs1] >> (uint32_t)imm;
                            print_inst(pc, "srai x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string(imm));
                            break;
                        default:
                            // todo: illegal instruction
                            break;
                    }
                    break;
                default:
                    // todo: illegal instruction
                    break;
            }
            
            if (rd != 0) reg[rd] = result;
            break;
        }
        case 0x33: { // REGISTER
                     // ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND
            uint32_t result;
            switch (funct3) {
                case 0x0: // ADD, SUB
                    switch (funct7) {
                        case 0x00: // ADD
                            result = (int32_t)reg[rs1] + (int32_t)reg[rs2];
                            print_inst(pc, "add x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
                            break;
                        case 0x20: // SUB
                            result = (int32_t)reg[rs1] - (int32_t)reg[rs2];
                            print_inst(pc, "sub x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
                            break;
                        default:
                            // todo: illegal instruction - funct7
                            break;
                    }
                    break;
                case 0x1: // SLL
                    if (funct7 & 0x7F) {
                        return;
                    }
                    result = (uint32_t)reg[rs1] <<((uint32_t)reg[rs2] & 0x1F); // No matter with reg[rs2][31:0]
                    print_inst(pc, "sll x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
                    break;
                case 0x2: // SLT
                    if (funct7 & 0x7F) {
                        return;
                    }
                    result = ((int32_t)reg[rs1] < (int32_t)reg[rs2]) ? 1 : 0;
                    print_inst(pc, "slt x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
                    break;
                case 0x3: // SLTU
                    if (funct7 & 0x7F) {
                        return;
                    }
                    result = ((uint32_t)reg[rs1] < (uint32_t)reg[rs2]) ? 1 : 0;
                    print_inst(pc, "sltu x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
                    break;
                case 0x4: // XOR
                    if (funct7 & 0x7F) {
                        return;
                    }
                    result = (uint32_t)reg[rs1] ^ (uint32_t)reg[rs2];
                    print_inst(pc, "xor x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
                    break;
                case 0x5: // SRL, SRAi
                    switch (funct7) {
                        case 0x00: // SRL
                            result = (uint32_t)reg[rs1] >> ((uint32_t)reg[rs2] & 0x1F);
                            print_inst(pc, "srl x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
                            break;
                        case 0x20: // SRA
                            result = (int32_t)reg[rs1] >> ((uint32_t)reg[rs2] & 0x1F);
                            print_inst(pc, "sra x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
                            break;
                        default:
                            // todo: illegal instruction - funct7
                            break;
                    }
                    break;
                case 0x6: // OR
                    if (funct7 & 0x7F) {
                        return;
                    }
                    result = (uint32_t)reg[rs1] | (uint32_t)reg[rs2];
                    print_inst(pc, "or x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
                    break;
                case 0x7: // AND
                    if (funct7 & 0x7F) {
                        return;
                    }
                    result = (uint32_t)reg[rs1] & (uint32_t)reg[rs2];
                    print_inst(pc, "and x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2));
                    break;
                default: // todo: illegal instruction - funct3
                    break;
            }
            if (rd != 0) reg[rd] = result;
        }
        default: // todo: illegal instruction - opcode
            break;
    }
}

int RISCV_32I::read_mem_8(uint32_t addr, uint8_t* data) {
    if (addr < 0 || addr >= MEM_SIZE) {
        return 0x10; // Memory out of bounds
    }
    *data = mem[addr - mem_start_addr];
    return 0;
}


