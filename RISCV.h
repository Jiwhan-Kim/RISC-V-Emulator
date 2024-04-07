#include <cstdint>
#include <string>

#define MEM_SIZE 0x10000

class RISCV {
    private:
        // 0 for allowing unaligned access, 1 for disallowing
        int mem_access_align;
        
        // 0 for disallowing debug mode, 1 for allowing
        int debug_mode;
    protected:
        uint8_t mem[MEM_SIZE] /* = {0, } */;
        
        int read_mem_u8(uint64_t addr, uint8_t* data);
        int read_mem_u16(uint64_t addr, uint16_t* data);
        int read_mem_u32(uint64_t addr, uint32_t* data);
        int read_mem_u64(uint64_t addr, uint64_t* data);
        int write_mem_u8(uint64_t addr, uint8_t data);
        int write_mem_u16(uint64_t addr, uint16_t data);
        int write_mem_u32(uint64_t addr, uint32_t data);
        int write_mem_u64(uint64_t addr, uint64_t data);
    public:
        RISCV();
        RISCV(int mem_access, int debug);

        void print_mem_all();
        void print_mem_u8(uint64_t addr);
        void print_mem_u16(uint64_t addr);
        void print_mem_u32(uint64_t addr);
        void print_mem_u64(uint64_t addr);

        void print_inst(uint64_t pc, std::string msg);
};

