#pragma once
#include <cstdint>

class ImmediateGen {
public:
    uint32_t decode(uint32_t instr) const noexcept;

    uint32_t immU(uint32_t instr) const noexcept;

private:
    static int32_t signExtend32(uint32_t x, int bits);

    uint32_t immI(uint32_t instr) const noexcept;
    uint32_t immS(uint32_t instr) const noexcept;
    uint32_t immB(uint32_t instr) const noexcept;
    uint32_t immJ(uint32_t instr) const noexcept;
};
