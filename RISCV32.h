#include <cstdint>
#include <string>

#define MEM_SIZE 0x10000
#define INSTR_ERR throw std::runtime_error("Invalid instruction")
#define MEM_ALIGN_ERR throw std::runtime_error("Unaligned memory access")
#define MEM_OUT_ERR throw std::runtime_error("Memory out of bounds")

class RISCV32 {
    private:
        // 0 for allowing unaligned access, 1 for disallowing
        static int mem_access_align;
        
        // 0 for disallowing debug mode, 1 for allowing
        static int debug_mode;
        
        // program counter
        static uint32_t pc;
        static uint32_t pc_next;

        static uint32_t reg32[32] /* = {0, } */;
          
        void execute32(uint32_t instr);
        
        class Memory32 {
            private:
                static uint8_t mem[MEM_SIZE] /* = {0, } */;
            
            public:
                static void read_mem_u8(uint32_t addr, uint8_t* data);
                static void read_mem_u16(uint32_t addr, uint16_t* data);
                static void read_mem_u32(uint32_t addr, uint32_t* data);
                static void write_mem_u8(uint32_t addr, uint8_t data);
                static void write_mem_u16(uint32_t addr, uint16_t data);
                static void write_mem_u32(uint32_t addr, uint32_t data);
               
                static void read_program(const char* program_file);

                static void print_mem_all();
                static void print_mem_u8(uint32_t addr);
                static void print_mem_u16(uint32_t addr);
                static void print_mem_u32(uint32_t addr);
        };
        static uint32_t imm_gen(uint32_t instr);

        // Instructions
        class base_I32 {
            public:
                // Instructions
                // U-type 
                static void lui(uint32_t rd, uint32_t imm);
                static void auipc(uint32_t rd, uint32_t imm);
                
                // J-type
                static void jal(uint32_t rd, uint32_t imm);
                
                // I-type
                static void jalr(uint32_t rd, uint32_t rs1, uint32_t imm);

                // B-type
                static void beq(uint32_t rs1, uint32_t rs2, uint32_t imm);
                static void bne(uint32_t rs1, uint32_t rs2, uint32_t imm);
                static void blt(uint32_t rs1, uint32_t rs2, uint32_t imm);
                static void bge(uint32_t rs1, uint32_t rs2, uint32_t imm);
                static void bltu(uint32_t rs1, uint32_t rs2, uint32_t imm);
                static void bgeu(uint32_t rs1, uint32_t rs2, uint32_t imm);

                // I-type
                static void lb(uint32_t rd, uint32_t rs1, uint32_t imm);
                static void lh(uint32_t rd, uint32_t rs1, uint32_t imm);
                static void lw(uint32_t rd, uint32_t rs1, uint32_t imm);
                static void lbu(uint32_t rd, uint32_t rs1, uint32_t imm);
                static void lhu(uint32_t rd, uint32_t rs1, uint32_t imm);
                
                // S-type
                static void sb(uint32_t rs1, uint32_t rs2, uint32_t imm);
                static void sh(uint32_t rs1, uint32_t rs2, uint32_t imm);
                static void sw(uint32_t rs1, uint32_t rs2, uint32_t imm);
                
                // I-type
                static void addi(uint32_t rd, uint32_t rs1, uint32_t imm);
                static void slti(uint32_t rd, uint32_t rs1, uint32_t imm);
                static void sltiu(uint32_t rd, uint32_t rs1, uint32_t imm);
                static void xori(uint32_t rd, uint32_t rs1, uint32_t imm);
                static void ori(uint32_t rd, uint32_t rs1, uint32_t imm);
                static void andi(uint32_t rd, uint32_t rs1, uint32_t imm);
                static void slli(uint32_t rd, uint32_t rs1, uint32_t shamt);
                static void srli(uint32_t rd, uint32_t rs1, uint32_t shamt);
                static void srai(uint32_t rd, uint32_t rs1, uint32_t shamt);

                // R-type
                static void add(uint32_t rd, uint32_t rs1, uint32_t rs2);
                static void sub(uint32_t rd, uint32_t rs1, uint32_t rs2);
                static void sll(uint32_t rd, uint32_t rs1, uint32_t rs2);
                static void slt(uint32_t rd, uint32_t rs1, uint32_t rs2);
                static void sltu(uint32_t rd, uint32_t rs1, uint32_t rs2);
                static void xor_(uint32_t rd, uint32_t rs1, uint32_t rs2);
                static void srl(uint32_t rd, uint32_t rs1, uint32_t rs2);
                static void sra(uint32_t rd, uint32_t rs1, uint32_t rs2);
                static void or_(uint32_t rd, uint32_t rs1, uint32_t rs2);
                static void and_(uint32_t rd, uint32_t rs1, uint32_t rs2);
        };
        class ext_M32 {
            private:
                // 0 for not extended, 1 for extended
                static bool extended;

            public:
                static void extend(bool ext);
        };
        class ext_A32 {
            private:
                // 0 for not extended, 1 for extended
                static bool extended;
            
            public:
                static void extend(bool ext);
        };
        class ext_F32 {
            private:
                // 0 for not extended, 1 for extended
                static bool extended;
            
            public:
                static void extend(bool ext);
        };

    public:
        RISCV32(
            bool mem_access, bool debug, bool M, bool A, bool F,
            const char* program_file, uint32_t mem_start, uint32_t entrypoint
        );
        void run();
        static void print_inst(uint32_t pc, std::string msg);
};

