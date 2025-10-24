#pragma once
#include <cstdint>

class Control {
public:
    static Control decode(uint32_t opcode_);

    bool regWrite() const { return regWrite_; }
    bool memRead()  const { return memRead_; }
    bool memWrite() const { return memWrite_; }
    bool memToReg() const { return memToReg_; }
    bool branch()   const { return branch_; }
    bool jump()     const { return jump_; }
    bool aluSrc()   const { return aluSrc_; }
    bool luiSrc()   const { return luiSrc_; }
    uint8_t aluOp() const { return aluOp_; }

private:
    bool    regWrite_{false};
    bool    memRead_{false};
    bool    memWrite_{false};
    bool    memToReg_{false};
    bool    branch_{false};
    bool    jump_{false};
    bool    aluSrc_{false};
    bool    luiSrc_{false};
    uint8_t aluOp_{0};
};
