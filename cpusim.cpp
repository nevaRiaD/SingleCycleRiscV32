// cpusim.cpp
#include "CPU.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>

static constexpr std::size_t MEM_BYTES = 4096;
static constexpr uint64_t MAX_CYCLES   = 50'000'000ULL; // hard stop to avoid runaway sims

// Reads hex bytes from a file: "b7" or "0xb7", commas optional, one byte per token/line.
static std::vector<uint8_t> readHexBytesPerLine(const std::string& path) {
    std::ifstream in(path);
    if (!in) return {};
    std::vector<uint8_t> bytes;
    std::string tok;
    while (in >> tok) {
        if (!tok.empty() && tok.back() == ',') tok.pop_back();
        if (tok.rfind("0x", 0) == 0 || tok.rfind("0X", 0) == 0) tok = tok.substr(2);
        if (tok.empty()) continue;
        unsigned long v = std::stoul(tok, nullptr, 16);
        bytes.push_back(static_cast<uint8_t>(v & 0xFFu));
    }
    return bytes;
}

// Little-endian 32-bit load from byte array at address a.
static inline uint32_t load32LE(const uint8_t* m, uint32_t a) {
    return (uint32_t)m[a]
         | ((uint32_t)m[a+1] << 8)
         | ((uint32_t)m[a+2] << 16)
         | ((uint32_t)m[a+3] << 24);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        // Require instruction file path; non-zero exit indicates usage error.
        return 1;
    }
    const std::string instPath = argv[1];

    // Load instruction bytes (program). Each 4 bytes form one RV32 instruction (LE).
    std::vector<uint8_t> prog = readHexBytesPerLine(instPath);
    if (prog.empty()) return 1;

    CPU cpu;
    cpu.loadInstrMem(prog.data(), std::min<std::size_t>(prog.size(), MEM_BYTES));
    cpu.setPC(0);
    cpu.setReg(2, MEM_BYTES); // x2 (sp) at end of RAM (optional convenience)

    uint64_t cycles = 0;
    while (true) {
        // Stop if fetch would be out of range.
        if (cpu.getPC() + 3 >= MEM_BYTES) break;

        // Fetch current instruction; stop on 0x00000000 sentinel.
        const uint32_t instr = load32LE(cpu.instrMem(), cpu.getPC());
        if (instr == 0x00000000u) break;

        // Execute one two-phase "single-cycle" step. Break if CPU requests stop.
        if (!cpu.step()) break;

        // Safety cap.
        if (++cycles > MAX_CYCLES) break;
    }

    // Print (a0, a1) as final sim output.
    const int a0 = static_cast<int>(cpu.getReg(10));
    const int a1 = static_cast<int>(cpu.getReg(11));
    std::cout << "(" << a0 << "," << a1 << ")\n";
    return 0;
}
