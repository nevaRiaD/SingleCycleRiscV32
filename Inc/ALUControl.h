#pragma once
#include <cstdint>

class ALUControl {
public:
    static ALUControl decode(uint8_t aluOp, uint32_t instr);

    uint8_t aluCtrl()  const { return aluCtrl_; }
    bool    bneCheck() const { return bneCheck_; }

private:
    enum : uint8_t {
        ALU_ADD  = 0b0000,
        ALU_SUB  = 0b0001,
        ALU_AND  = 0b0010,
        ALU_OR   = 0b0011,
        ALU_SLTU = 0b0100,
        ALU_SRA  = 0b0101,
        ALU_NOP  = 0b1111
    };

    static uint8_t rviOp(uint8_t f3, uint8_t b30);
    static uint8_t rviITypeOp(uint8_t f3, uint8_t b30);

    uint8_t aluCtrl_{ALU_NOP};
    bool    bneCheck_{false};
};
