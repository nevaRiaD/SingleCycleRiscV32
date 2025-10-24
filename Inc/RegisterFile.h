#pragma once
#include <cstdint>
#include <array>

class RegisterFile {
public:
    RegisterFile();

    uint32_t readScalar(uint8_t r) const;
    void     writeScalar(uint8_t r, uint32_t v);

    void readPair(uint8_t rs1, uint8_t rs2,
                  uint32_t& out1, uint32_t& out2,
                  bool wbEn, uint8_t wbRd, uint32_t wbData) const;

    void commitWrite(uint8_t rd, uint32_t data);

private:
    std::array<uint32_t, 32> regs_{};
};
