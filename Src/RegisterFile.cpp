#include "RegisterFile.h"

// Initialize x0..x31 to zero.
RegisterFile::RegisterFile() { regs_.fill(0); }

// Read scalar register; x0 is always 0.
uint32_t RegisterFile::readScalar(uint8_t r) const { return r ? regs_[r] : 0u; }

// Write scalar register; writes to x0 are ignored.
void     RegisterFile::writeScalar(uint8_t r, uint32_t v) { if (r) regs_[r] = v; }

// Read two regs with simple WB bypass (transparent read-after-write for prior instr).
void RegisterFile::readPair(uint8_t rs1, uint8_t rs2,
                            uint32_t& out1, uint32_t& out2,
                            bool wbEn, uint8_t wbRd, uint32_t wbData) const {
    out1 = readScalar(rs1);
    out2 = readScalar(rs2);
    if (wbEn && wbRd) {
        if (rs1 == wbRd) out1 = wbData;
        if (rs2 == wbRd) out2 = wbData;
    }
}

// Commit write in the "commit" phase; x0 remains hard-wired to zero.
void RegisterFile::commitWrite(uint8_t rd, uint32_t data) {
    if (rd) regs_[rd] = data;
}
