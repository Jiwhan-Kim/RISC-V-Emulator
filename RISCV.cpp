#include "RISCV.h"
#include <iostream>
#include <string>

RISCV::RISCV() {
    mem_access_align = 0;
    debug_mode = 0;
}

RISCV::RISCV(int mem_access, int debug) {
    mem_access_align = mem_access;
    debug_mode = debug;
}

int RISCV::read_mem_u8(uint64_t addr, uint8_t* data) {
    if (addr >= MEM_SIZE) {
        // todo: error handling
        return 1; // out of range
    }
    *data = mem[addr];
    return 0;
}

int RISCV::read_mem_u16(uint64_t addr, uint16_t* data) {
    if (addr >= MEM_SIZE - 1) {
        // todo: error handling
        return 1; // out of range
    }
    if (mem_access_align && addr & 1) {
        return 1; // unaligned access
    }
    *data = mem[addr] | ((uint16_t)mem[addr + 1] << 8);
    return 0;
}

int RISCV::read_mem_u32(uint64_t addr, uint32_t* data) {
    if (addr >= MEM_SIZE - 3) {
        // todo: error handling
        return 1; // out of range
    }
    if (mem_access_align && addr & 3) {
        return 1; // unaligned access
    }
    *data = (uint32_t)mem[addr] | ((uint32_t)mem[addr + 1] << 8) | ((uint32_t)mem[addr + 2] << 16) | ((uint32_t)mem[addr + 3] << 24);
    return 0;
}

int RISCV::read_mem_u64(uint64_t addr, uint64_t* data) {
    if (addr >= MEM_SIZE - 7) {
        // todo: error handling
        return 1; // out of range
    }
    if (mem_access_align && addr & 7) {
        return 1; // unaligned access
    }
    *data = (uint64_t)mem[addr] | ((uint64_t)mem[addr + 1] << 8) | ((uint64_t)mem[addr + 2] << 16) | ((uint64_t)mem[addr + 3] << 24) | ((uint64_t)mem[addr + 4] << 32) | ((uint64_t)mem[addr + 5] << 40) | ((uint64_t)mem[addr + 6] << 48) | ((uint64_t)mem[addr + 7] << 56);
    return 0;
}

int RISCV::write_mem_u8(uint64_t addr, uint8_t data) {
    if (addr >= MEM_SIZE) {
        // todo: error handling
        return 1; // out of range
    }
    mem[addr] = data;
    return 0;
}

int RISCV::write_mem_u16(uint64_t addr, uint16_t data) {
    if (addr >= MEM_SIZE - 1) {
        // todo: error handling
        return 1; // out of range
    }
    if (mem_access_align && addr & 1) {
        return 1; // unaligned access
    }
    mem[addr] = data & 0xff;
    mem[addr + 1] = (data >> 8) & 0xff;
    return 0;
}

int RISCV::write_mem_u32(uint64_t addr, uint32_t data) {
    if (addr >= MEM_SIZE - 3) {
        // todo: error handling
        return 1; // out of range
    }
    if (mem_access_align && addr & 3) {
        return 1; // unaligned access
    }
    mem[addr] = data & 0xff;
    mem[addr + 1] = (data >> 8) & 0xff;
    mem[addr + 2] = (data >> 16) & 0xff;
    mem[addr + 3] = (data >> 24) & 0xff;
    return 0;
}

int RISCV::write_mem_u64(uint64_t addr, uint64_t data) {
    if (addr >= MEM_SIZE - 7) {
        // todo: error handling
        return 1; // out of range
    }
    if (mem_access_align && addr & 7) {
        return 1; // unaligned access
    }
    mem[addr] = data & 0xff;
    mem[addr + 1] = (data >> 8) & 0xff;
    mem[addr + 2] = (data >> 16) & 0xff;
    mem[addr + 3] = (data >> 24) & 0xff;
    mem[addr + 4] = (data >> 32) & 0xff;
    mem[addr + 5] = (data >> 40) & 0xff;
    mem[addr + 6] = (data >> 48) & 0xff;
    mem[addr + 7] = (data >> 56) & 0xff;
    return 0;
}

// public
void RISCV::print_mem_all() {
    std::cout << "Memory" << std::endl;
    std::cout << "     Address     " << " | " << "  Data  " << std::endl;
    std::cout << "-----------------" << " | " << "--------" << std::endl;
    for (int i = 0; i < MEM_SIZE / 4; i++) {
        std::cout.width(17);
        std::cout.fill('0');
        std::cout << std::hex << i * 4 << " | ";
        for (int j = 3; j >= 0; j--) {
            std::cout << std::hex << (int)mem[i * 4 + j];
        }
        std::cout << std::endl;
    }
}

void RISCV::print_mem_u8(uint64_t addr) {
    uint8_t data;
    if (read_mem_u8(addr, &data)) {
        std::cout << "Error: out of range" << std::endl;
        return;
    }
    std::cout << "mem[[" << std::hex << addr << "] = " << std::hex << (int)data << std::endl;
}

void RISCV::print_mem_u16(uint64_t addr) {
    uint16_t data;
    if (read_mem_u16(addr, &data)) {
        std::cout << "Error: out of range" << std::endl;
        return;
    }
    if (mem_access_align && addr & 1) {
        std::cout << "Error: unaligned access" << std::endl;
        return;
    }
    std::cout << "mem[[" << std::hex << addr << "] = " << std::hex << data << std::endl;
}

void RISCV::print_mem_u32(uint64_t addr) {
    uint32_t data;
    if (read_mem_u32(addr, &data)) {
        std::cout << "Error: out of range" << std::endl;
        return;
    }
    if (mem_access_align && addr & 3) {
        std::cout << "Error: unaligned access" << std::endl;
        return;
    }
    std::cout << "mem[[" << std::hex << addr << "] = " << std::hex << data << std::endl;
}

void RISCV::print_mem_u64(uint64_t addr) {
    uint64_t data;
    if (read_mem_u64(addr, &data)) {
        std::cout << "Error: out of range" << std::endl;
        return;
    }
    if (mem_access_align && addr & 7) {
        std::cout << "Error: unaligned access" << std::endl;
        return;
    }
    std::cout << "mem[[" << std::hex << addr << "] = " << std::hex << data << std::endl;
}

void RISCV::print_inst(uint64_t pc, std::string msg) {
    if (debug_mode == 0) return;
    std::cout << "pc: ";
    std::cout.width(16);
    std::cout << std::hex << pc << " : ";
    std::cout << msg << std::endl;
}
