#pragma once
#include <cstdint>

class ALU {
public:
    static ALU decode(uint32_t rs1Val, uint32_t rs2Val, uint32_t imm,
                      uint8_t aluSel, bool aluSrc, bool luiSrc);

    uint32_t aluResult()  const { return result_; }
    bool     branchZero() const { return result_ == 0; }

private:
    ALU(uint32_t a, uint32_t b, uint8_t sel);

    enum : uint8_t {
        ALU_ADD  = 0b0000,
        ALU_SUB  = 0b0001,
        ALU_AND  = 0b0010,
        ALU_OR   = 0b0011,
        ALU_SLTU = 0b0100,
        ALU_SRA  = 0b0101,
        ALU_NOP  = 0b1111
    };

    uint32_t result_{0};
};
