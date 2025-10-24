#include "InstructionMem.h"

// Extract bits [hi:lo] inclusive.
static inline uint32_t bits(uint32_t x, int hi, int lo) {
    return (x >> lo) & ((1u << (hi - lo + 1)) - 1u);
}

// Shallow decode of instruction fields used by other units.
// (Immediates are left to ImmediateGen.)
InstructionMemory InstructionMemory::decode(uint32_t instr) {
    InstructionMemory s;
    s.instrMem_  = instr;

    // RV32I fields for downstream units:
    s.instrCon_  = bits(instr, 6, 0);    // opcode -> Control
    s.instrWreg_ = bits(instr, 11, 7);   // rd     -> Register File (write reg)
    s.instrReg1_ = bits(instr, 19, 15);  // rs1    -> Register File (read 1)
    s.instrReg2_ = bits(instr, 24, 20);  // rs2    -> Register File (read 2)

    // Immediate value is kept as raw instr; ImmediateGen will parse it.
    s.instrImm_  = instr;

    return s;
}
