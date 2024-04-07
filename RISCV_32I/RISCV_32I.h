#include "../RISCV.h"
#include <cstdint>
#define MEM_SIZE 0x10000

class RISCV_32I: public RISCV {
    private:
        uint8_t mem[MEM_SIZE]; // no initialization
        uint32_t mem_start_addr = 0x0;
        
        // IF Stage
        uint32_t pc = 0x0;
        uint32_t next_pc = 0x0;

        // ID Stage
        uint32_t reg[32]; // no initialization

        // EX Stage
        uint32_t alu_out = 0x0;

        // MEM Stage
        // WB Stage
        
        void inst_execute(uint32_t instr);
        
        int read_mem_8(uint32_t addr, uint8_t* data);
        int read_mem_16(uint32_t addr, uint16_t* data);
        int read_mem_32(uint32_t addr, uint32_t* data);
        int write_mem_8(uint32_t addr, uint8_t data);
        int write_mem_16(uint32_t addr, uint16_t data);
        int write_mem_32(uint32_t addr, uint32_t data);

        void print_mem();
        void print_reg();
    public:
        RISCV_32I(const char* filename, uint32_t mem_start, uint32_t entrypoint);
        void run();
};
