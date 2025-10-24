#include "ImmediateGen.h"

// Sign-extend x having 'bits' width to 32 bits.
int32_t ImmediateGen::signExtend32(uint32_t x, int bits) {
    const uint32_t m = 1u << (bits - 1);
    return static_cast<int32_t>((x ^ m) - m);
}

// Choose immediate format based on opcode.
uint32_t ImmediateGen::decode(uint32_t instr) const noexcept {
    const uint32_t opcode = instr & 0x7F;
    switch (opcode) {
        case 0b0010011: // I-type arith
        case 0b0000011: // Loads
        case 0b1100111: // JALR
            return immI(instr);
        case 0b0100011: // Stores
            return immS(instr);
        case 0b1100011: // Branches
            return immB(instr);
        case 0b0110111: // LUI
        case 0b0010111: // AUIPC
            return immU(instr);
        case 0b1101111: // JAL
            return immJ(instr);
        default:
            return 0; // unknown -> 0
    }
}

// U-type: imm in [31:12], lower 12 bits zeroed.
uint32_t ImmediateGen::immU(uint32_t instr) const noexcept {
    return instr & 0xFFFFF000u;
}

// I-type: sign-extend bits [31:20].
uint32_t ImmediateGen::immI(uint32_t instr) const noexcept {
    uint32_t raw = (instr >> 20) & 0xFFFu;
    return static_cast<uint32_t>(signExtend32(raw, 12));
}

// S-type: split across [31:25] and [11:7], sign-extend.
uint32_t ImmediateGen::immS(uint32_t instr) const noexcept {
    uint32_t imm11_5 = (instr >> 25) & 0x7Fu;
    uint32_t imm4_0  = (instr >> 7)  & 0x1Fu;
    uint32_t raw = (imm11_5 << 5) | imm4_0;
    return static_cast<uint32_t>(signExtend32(raw, 12));
}

// B-type: assembled with implied bit0=0, sign-extend 13-bit offset.
uint32_t ImmediateGen::immB(uint32_t instr) const noexcept {
    uint32_t imm12   = (instr >> 31) & 0x1u;
    uint32_t imm10_5 = (instr >> 25) & 0x3Fu;
    uint32_t imm4_1  = (instr >> 8)  & 0xFu;
    uint32_t imm11   = (instr >> 7)  & 0x1u;
    uint32_t raw = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);
    int32_t off = signExtend32(raw, 13);
    return static_cast<uint32_t>(off);
}

// J-type: assembled with implied bit0=0, sign-extend 21-bit offset.
uint32_t ImmediateGen::immJ(uint32_t instr) const noexcept {
    uint32_t imm20    = (instr >> 31) & 0x1u;
    uint32_t imm10_1  = (instr >> 21) & 0x3FFu;
    uint32_t imm11    = (instr >> 20) & 0x1u;
    uint32_t imm19_12 = (instr >> 12) & 0xFFu;
    uint32_t raw = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);
    int32_t off = signExtend32(raw, 21);
    return static_cast<uint32_t>(off);
}
