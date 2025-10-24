#include "ALU.h"

// Build ALU from control + selected operands.
// - aluSrc chooses imm vs rs2
// - luiSrc zeros op1 for LUI-like behavior
ALU ALU::decode(uint32_t rs1Val, uint32_t rs2Val, uint32_t imm,
                uint8_t aluSel, bool aluSrc, bool luiSrc)
{
    uint32_t op1 = luiSrc ? 0u : rs1Val; // LUI: op1 = 0 so result = imm
    uint32_t op2 = aluSrc ? imm : rs2Val;
    return ALU(op1, op2, aluSel);
}

// Execute selected ALU operation; undefined ops return 0.
ALU::ALU(uint32_t a, uint32_t b, uint8_t sel) {
    switch (sel) {
        case ALU_ADD:  result_ = a + b; break;
        case ALU_SUB:  result_ = a - b; break;
        case ALU_AND:  result_ = a & b; break;
        case ALU_OR:   result_ = a | b; break;
        case ALU_SLTU: result_ = (a < b) ? 1u : 0u; break;                // unsigned compare
        case ALU_SRA:  result_ = static_cast<uint32_t>(static_cast<int32_t>(a) >> (b & 0x1F)); break; // arithmetic shift
        default:       result_ = 0; break;
    }
}
