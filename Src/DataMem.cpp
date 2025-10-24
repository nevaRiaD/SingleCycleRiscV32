#include "DataMem.h"

// Wrap address to memory size (power-of-two assumed).
static inline uint32_t maskA(uint32_t a, std::size_t n) { return a & (static_cast<uint32_t>(n) - 1u); }

// Pure compute stage for data memory: performs reads and prepares a store (no writes yet).
DataMemResult DataMem::compute(uint32_t addrMasked, uint32_t rs2Val,
                               bool memWrite, bool memRead, bool /*memToReg*/,
                               uint8_t* mem, std::size_t memBytes,
                               uint8_t funct3Load, uint8_t funct3Store)
{
    DataMemResult res;

    // Handle loads (LB/LBU/LH/LHU/LW).
    if (memRead) {
        uint32_t a = maskA(addrMasked, memBytes);
        uint8_t b0 = mem[a];
        uint32_t r = 0;

        // Load Unit
        switch (funct3Load) {
            case 0b000: // LB  (sign-extend byte)
                r = static_cast<int32_t>(static_cast<int8_t>(b0));
                break;
            case 0b100: // LBU (zero-extend byte)
                r = static_cast<uint32_t>(b0);
                break;
            case 0b001: // LH
            case 0b101: { // LHU
                uint16_t h = static_cast<uint16_t>(mem[a]) |
                             (static_cast<uint16_t>(mem[maskA(a + 1, memBytes)]) << 8);
                r = (funct3Load == 0b001)
                    ? static_cast<uint32_t>(static_cast<int32_t>(static_cast<int16_t>(h))) // sign-extend
                    : static_cast<uint32_t>(h); // zero-extend
                break;
            }
            case 0b010: { // LW (32-bit little-endian)
                uint32_t w = static_cast<uint32_t>(mem[a]) |
                             (static_cast<uint32_t>(mem[maskA(a + 1, memBytes)]) << 8) |
                             (static_cast<uint32_t>(mem[maskA(a + 2, memBytes)]) << 16) |
                             (static_cast<uint32_t>(mem[maskA(a + 3, memBytes)]) << 24);
                r = w;
                break;
            }
            default: r = 0; break; // unsupported size -> 0
        }
        res.setRead(r);
    }

    // Prepare store operation (SB/SH/SW). Actual write happens in commit.
    if (memWrite) {
        DataStoreOp s{};
        s.en   = true;
        s.addr = addrMasked;
        switch (funct3Store) {
            case 0b000: s.size = 1; s.data = rs2Val & 0xFFu;    break; // SB
            case 0b001: s.size = 2; s.data = rs2Val & 0xFFFFu;  break; // SH
            case 0b010: s.size = 4; s.data = rs2Val;            break; // SW
            default: s.en = false; s.size = 0; s.data = 0;      break; // ignore unsupported
        }
        res.setStore(s);
    }

    return res;
}

// Commit the staged store into memory (side-effect happens here).
void DataMem::commitStore(const DataStoreOp& s, uint8_t* mem, std::size_t memBytes) {
    if (!s.en) return;
    uint32_t a = maskA(s.addr, memBytes);
    switch (s.size) {
        case 1:
            mem[a] = static_cast<uint8_t>(s.data & 0xFFu);
            break;
        case 2:
            mem[a] = static_cast<uint8_t>(s.data & 0xFFu);
            mem[maskA(a + 1, memBytes)] = static_cast<uint8_t>((s.data >> 8) & 0xFFu);
            break;
        case 4:
            mem[a] = static_cast<uint8_t>(s.data & 0xFFu);
            mem[maskA(a + 1, memBytes)] = static_cast<uint8_t>((s.data >> 8) & 0xFFu);
            mem[maskA(a + 2, memBytes)] = static_cast<uint8_t>((s.data >> 16) & 0xFFu);
            mem[maskA(a + 3, memBytes)] = static_cast<uint8_t>((s.data >> 24) & 0xFFu);
            break;
        default: break; // no-op
    }
}
