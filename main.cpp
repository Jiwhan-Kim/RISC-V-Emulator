#include <iostream>
#include "RISCV32.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << "<filename> [entry point] <mode> "<< std::endl;
        return 1;
    }

    uint32_t entry_point = 0x0;
    if (argc >= 3) {
        entry_point = std::stoi(argv[2], nullptr, 16);
    }
    
    bool mem_access = false;
    bool debug = false; 
    bool M, A, F = false;
    if (argc >= 4) {
        std::string flags = argv[3];
        if (flags.find('m') != std::string::npos) {
            mem_access = true;
        }
        if (flags.find('d') != std::string::npos) {
            debug = true;
        }
        if (flags.find('M') != std::string::npos) {
            M = true;
        }
        if (flags.find('A') != std::string::npos) {
            A = true;
        }
        if (flags.find('F') != std::string::npos) {
            F = true;
        }
    }
    try {
        RISCV32 hart {
            mem_access, debug, M, A, F,
            argv[1], 0, entry_point
        };
        hart.run();
    } catch (std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}


