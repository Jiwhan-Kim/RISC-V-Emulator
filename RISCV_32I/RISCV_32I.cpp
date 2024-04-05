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
        case 0x37: // LUI
            imm = (instr & 0xFFFFF000);
            break;
        case 0x17: // AUIPC
            imm = (instr & 0xFFFFF000);
            break;
        case 0x6F: // JAL
            //  = ([31] sign_bit) | ([30:20] sign) | :
            imm = (instr & 0x80000000) | ((instr >> 20) & 0xFFE00000) | ((instr >> 9) & 0x800) | (instr & 0xFF);
            break;
        case 0x67: // JALR
                   // funct3 = 0x0
            imm = (instr >> 20);
            break;
        case 0x63: // BRANCH
                   // BEQ, BNE, BLT, BGE, BLTU, BGEU
            imm = (instr & 0x80000000) | ((instr >> 7) & 0xF00) | ((instr >> 20) & 0x7FE) | ((instr >> 20) & 0x80);
            break;
        case 0x03: // LOAD
                   // LB, LH, LW, LBU, LHU
            break;
        case 0x23: // STORE
                   // SB, SH, SW
            break;
        case 0x13: // IMMEDIATE
                   // ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI
            break;
        case 0x33: // REGISTER
                   // ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND
            break;
        default:
            break;
    }


}
