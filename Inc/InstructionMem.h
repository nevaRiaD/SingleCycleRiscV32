#pragma once
#include <cstdint>

class InstructionMemory {
private:
    uint32_t instrMem_ = 0;
    uint32_t instrCon_ = 0;
    uint32_t instrReg1_ = 0;
    uint32_t instrReg2_ = 0;
    uint32_t instrWreg_ = 0;
    uint32_t instrImm_ = 0;

public:
    static InstructionMemory decode(uint32_t instrMem_); // pc address read by separate function and 32 bit value passed here

    uint32_t instrCon() const { return instrCon_; }
    uint32_t instrReg1() const { return instrReg1_; }
    uint32_t instrReg2() const { return instrReg2_; }
    uint32_t instrWreg() const { return instrWreg_; }
    uint32_t instrImm() const { return instrImm_; }

    InstructionMemory() = default;
};