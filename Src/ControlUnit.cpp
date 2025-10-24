#include "ControlUnit.h"

// Decode opcode into control signals for datapath selection.
// Signals intentionally minimal: regWrite, memRead/Write, memToReg, aluSrc, branch, jump, luiSrc, aluOp.
Control Control::decode(uint32_t opcode_) {
    Control c{};
    const uint32_t opcode = opcode_ & 0x7F;

    switch (opcode) {
        case 0b0110011: // R-type (register ALU ops)
            c.regWrite_ = true;  c.aluSrc_ = false; c.aluOp_ = 2; break;
        case 0b0010011: // I-type ALU (ADDI/ORI/SLTIU/...)
            c.regWrite_ = true;  c.aluSrc_ = true;  c.aluOp_ = 3; break;
        case 0b0000011: // Loads
            c.regWrite_ = true;  c.memRead_ = true; c.memToReg_ = true; c.aluSrc_ = true; c.aluOp_ = 0; break;
        case 0b0100011: // Stores
            c.memWrite_ = true;  c.aluSrc_ = true;  c.aluOp_ = 0; break;
        case 0b1100011: // Branches (BEQ/BNE)
            c.branch_   = true;  c.aluSrc_ = false; c.aluOp_ = 1; break;
        case 0b1101111: // JAL
            c.jump_     = true;  c.regWrite_ = true; c.aluSrc_ = true;  c.aluOp_ = 0; break;
        case 0b1100111: // JALR
            c.jump_     = true;  c.regWrite_ = true; c.aluSrc_ = true;  c.aluOp_ = 0; break;
        case 0b0110111: // LUI (rd = immU)
            c.regWrite_ = true;  c.luiSrc_ = true;   c.aluSrc_ = true;  c.aluOp_ = 0; break;
        case 0b0010111: // AUIPC (rd = PC + immU)
            c.regWrite_ = true;  c.aluSrc_ = true;   c.aluOp_ = 0; break;
        default: break; // unsupported -> all zeros (NOP)
    }
    return c;
}
