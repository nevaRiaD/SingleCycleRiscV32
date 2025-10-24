#include "ALUControl.h"

// Translate high-level aluOp + instr fields into a concrete ALU operation.
// Also sets bneCheck_ so branch comparator can flip BEQ/BNE sense.
ALUControl ALUControl::decode(uint8_t aluOp, uint32_t instr) {
    ALUControl c{};
    const uint8_t funct3 = static_cast<uint8_t>((instr >> 12) & 0x7);
    const uint8_t bit30  = static_cast<uint8_t>((instr >> 30) & 0x1);

    switch (aluOp) {
        case 0: c.aluCtrl_ = ALU_ADD; break;                                   // address calc (loads/stores), AUIPC
        case 1: c.aluCtrl_ = ALU_SUB; c.bneCheck_ = (funct3 == 0b001); break;  // BEQ/BNE (SUB+zero check; flip for BNE)
        case 2: c.aluCtrl_ = rviOp(funct3, bit30); break;                       // R-type
        case 3: c.aluCtrl_ = rviITypeOp(funct3, bit30); break;                  // I-type ALU
        default: c.aluCtrl_ = ALU_NOP; break;
    }
    return c;
}

// Map R-type funct3/bit30 to ALU op.
uint8_t ALUControl::rviOp(uint8_t f3, uint8_t b30) {
    switch (f3) {
        case 0b000: return b30 ? ALU_SUB : ALU_ADD; // ADD/SUB
        case 0b111: return ALU_AND;
        case 0b110: return ALU_OR;
        case 0b011: return ALU_SLTU;
        case 0b101: return (b30 ? ALU_SRA : ALU_NOP); // SRA/SRL (only SRA used)
        default:    return ALU_NOP;
    }
}

// Map I-type funct3/bit30 to ALU op (subset used).
uint8_t ALUControl::rviITypeOp(uint8_t f3, uint8_t b30) {
    switch (f3) {
        case 0b000: return ALU_ADD;   // ADDI
        case 0b110: return ALU_OR;    // ORI
        case 0b011: return ALU_SLTU;  // SLTIU
        case 0b101: return (b30 ? ALU_SRA : ALU_NOP); // SRAI/SRLI -> using SRAI
        default:    return ALU_NOP;
    }
}
