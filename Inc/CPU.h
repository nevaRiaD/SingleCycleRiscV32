
#pragma once
#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>

#include "RegisterFile.h"
#include "ALU.h"
#include "ALUControl.h"
#include "ImmediateGen.h"
#include "ControlUnit.h"
#include "DataMem.h"

class CPU {
public:
    // Memories are simple byte arrays owned by CPU
    static constexpr std::size_t MEM_BYTES = 4096;

    CPU();

    // Loaders
    void loadInstrMem(const uint8_t* src, std::size_t nbytes);
    void loadDataMem(const uint8_t* src, std::size_t nbytes);

    // Run one clock tick (two-phase: compute then commit). Returns false if PC would go out of range.
    bool step();

    // Accessors
    uint32_t getReg(uint8_t r) const { return rf_.readScalar(r); }
    void     setReg(uint8_t r, uint32_t v) { rf_.writeScalar(r, v); }
    uint32_t getPC() const { return pc_; }
    void     setPC(uint32_t v) { pc_ = v; }

    const uint8_t* instrMem() const { return imem_.data(); }
    const uint8_t* dataMem () const { return dmem_.data(); }
          uint8_t* dataMem ()       { return dmem_.data(); }

private:
    // ========== State ==========
    uint32_t       pc_ = 0;                 // byte address
    std::array<uint8_t, MEM_BYTES> imem_{}; // instruction memory (read-only here)
    std::array<uint8_t, MEM_BYTES> dmem_{}; // data memory

    RegisterFile rf_;

    // WB latch from previous cycle (used for transparent bypass on reads)
    struct WBLatch {
        bool     en = false;
        uint8_t  rd = 0;
        uint32_t data = 0;
    } wb_;

    // Staged store (from DataMem::compute) to commit in Phase B
    DataStoreOp stagedStore_;

    // ========== Helpers ==========
    static inline uint32_t load32LE(const uint8_t* m, uint32_t a) {
        return (uint32_t)m[a] | ((uint32_t)m[a+1] << 8) | ((uint32_t)m[a+2] << 16) | ((uint32_t)m[a+3] << 24);
    }

    static inline uint32_t bits(uint32_t x, int hi, int lo) {
        return (x >> lo) & ((1u << (hi - lo + 1)) - 1u);
    }

    // Compute next PC given control and immediates
    uint32_t computeNextPC(uint32_t pc, bool jump, bool branchTaken, uint32_t imm, uint32_t rs1Val, bool isJalr) const;
};