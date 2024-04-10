#include "RISCV32.h"
#include <iostream>
#include <string>

RISCV32::RISCV32(int mem_access, int debug, bool M, bool A, bool F) {
    mem_access_align = mem_access;
    debug_mode = debug;
    
}

void RISCV32::run() {

}

void RISCV32::print_inst(uint32_t pc, std::string msg) {
    if (debug_mode == 0) return;
    std::cout << "pc: ";
    std::cout.width(8);
    std::cout << pc << " " << msg << std::endl;
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
            base_I32::lui(rd, instr);
        } break;

        case 0x17: {
            base_I32::auipc(rd, instr);
        } break;

        case 0x6F: {
            base_I32::jal(rd, instr);
        } break;

        case 0x67: {
            base_I32::jalr(rd, rs1, instr);
        } break;

        case 0x63: {
            switch (funct3) {
                case 0x0: {
                    base_I32::beq(rs1, rs2, instr);
                } break;
                case 0x1: {
                    base_I32::bne(rs1, rs2, instr);
                } break;
                case 0x4: {
                    base_I32::blt(rs1, rs2, instr);
                } break;
                case 0x5: {
                    base_I32::bge(rs1, rs2, instr);
                } break;
                case 0x6: {
                    base_I32::bltu(rs1, rs2, instr);
                } break;
                case 0x7: {
                    base_I32::bgeu(rs1, rs2, instr);
                } break;
                default: {
                    INSTR_ERR;
                } break;
            }
        } break;

        case 0x03: {
            switch (funct3) {
                case 0x0: {
                    base_I32::lb(rd, rs1, instr);
                } break;
                case 0x1: {
                    base_I32::lh(rd, rs1, instr);
                } break;
                case 0x2: {
                    base_I32::lw(rd, rs1, instr);
                } break;
                case 0x4: {
                    base_I32::lbu(rd, rs1, instr);
                } break;
                case 0x5: {
                    base_I32::lhu(rd, rs1, instr);
                } break;
                default: {
                    INSTR_ERR;
                } break;
            }
        } break;

        case 0x23: {
            switch (funct3) {
                case 0x0: {
                    base_I32::sb(rs1, rs2, instr);
                } break;
                case 0x1: {
                    base_I32::sh(rs1, rs2, instr);
                } break;
                case 0x2: {
                    base_I32::sw(rs1, rs2, instr);
                } break;
                default: {
                    INSTR_ERR;
                } break;
            }
        } break;

        case 0x13: {
            switch (funct3) {
                case 0x0: {
                    base_I32::addi(rd, rs1, instr);
                } break;
                case 0x2: {
                    base_I32::slti(rd, rs1, instr);
                } break;
                case 0x3: {
                    base_I32::sltiu(rd, rs1, instr);
                } break;
                case 0x4: {
                    base_I32::xori(rd, rs1, instr);
                } break;
                case 0x6: {
                    base_I32::ori(rd, rs1, instr);
                } break;
                case 0x7: {
                    base_I32::andi(rd, rs1, instr);
                } break;
                case 0x1: {
                    base_I32::slli(rd, rs1, instr);
                } break;
                case 0x5: {
                    switch (funct7) {
                        case 0x00: {
                            base_I32::srli(rd, rs1, instr);
                        } break;
                        case 0x20: {
                            base_I32::srai(rd, rs1, instr);
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
    std::cout << "Memory[" << addr << "]: " << std::hex << data << std::endl;
}

void RISCV32::Memory32::print_mem_u32(uint32_t addr) {
    uint32_t data;
    read_mem_u32(addr, &data);
    std::cout << "Memory[" << addr << "]: " << std::hex << data << std::endl;
}

// Instructions
RISCV32::base_I32::base_I32() {
    // Nothing to do..!
}

// R-type
void RISCV32::base_I32::add(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(0, "add " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(rs2));
    }
    if (rd == 0) return;
    reg32[rd] = reg32[rs1] + reg32[rs2];
}

void RISCV32::base_I32::sub(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (debug_mode == 1) {
        print_inst(0, "sub " + std::to_string(rd) + ", " + std::to_string(rs1) + ", " + std::to_string(rs2));
    }
    if (rd == 0) return;
    reg32[rd] = reg32[rs1] - reg32[rs2];
}


