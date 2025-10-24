#include "CPU.h"
#include <cstring>
#include <algorithm>

// ========================= CPU (two-phase single-cycle) =========================

CPU::CPU() {
    std::fill(imem_.begin(), imem_.end(), 0);
    std::fill(dmem_.begin(), dmem_.end(), 0);
    pc_ = 0;
    wb_ = {};            // prior write-back latch (for transparent bypass)
    stagedStore_.clear();// pending store (committed in Phase B)
}

// Copy program bytes into instruction memory.
void CPU::loadInstrMem(const uint8_t* src, std::size_t nbytes) {
    if (!src) return;
    if (nbytes > MEM_BYTES) nbytes = MEM_BYTES;
    std::memcpy(imem_.data(), src, nbytes);
}

// Optional loader for data memory (e.g., initial RAM image).
void CPU::loadDataMem(const uint8_t* src, std::size_t nbytes) {
    if (!src) return;
    if (nbytes > MEM_BYTES) nbytes = MEM_BYTES;
    std::memcpy(dmem_.data(), src, nbytes);
}

// Compute next PC based on jump/branch decision and immediate.
// JALR clears bit0 per spec; branches/jumps use PC-relative targets.
uint32_t CPU::computeNextPC(uint32_t pc, bool jump, bool branchTaken, uint32_t imm, uint32_t rs1Val, bool isJalr) const {
    if (jump) {
        if (isJalr) {
            // JALR: target = (rs1 + imm) & ~1
            return (rs1Val + imm) & ~1u;
        } else {
            // JAL: target = pc + imm
            return pc + imm;
        }
    }
    if (branchTaken) {
        return pc + imm;
    }
    return pc + 4; // sequential
}

// Mask effective address to 4 KiB (wrap-around).
static inline uint32_t effAddrMask(uint32_t a) {
    // 4KB wrap: keep low 12 bits
    return a & (CPU::MEM_BYTES - 1);
}

// Execute one "cycle": Phase A (compute) then Phase B (commit).
bool CPU::step() {
    // Need 4 bytes to fetch
    if (pc_ + 3 >= MEM_BYTES) return false;

    // ===== Phase A: compute (no state updates) =====
    const uint32_t instr = load32LE(imem_.data(), pc_);

    // Decode primary fields
    const uint32_t opcode = bits(instr, 6, 0);
    const uint8_t  rd     = (uint8_t)bits(instr, 11, 7);
    const uint8_t  funct3 = (uint8_t)bits(instr, 14, 12);
    const uint8_t  rs1    = (uint8_t)bits(instr, 19, 15);
    const uint8_t  rs2    = (uint8_t)bits(instr, 24, 20);
    // const uint8_t  bit30  = (uint8_t)bits(instr, 30, 30); // available if needed

    // Global control (based on opcode)
    const Control ctrl = Control::decode(instr);

    // Immediate value for this instruction
    ImmediateGen ig;
    const uint32_t imm = ig.decode(instr);

    // ALU control (opcode + funct fields -> specific ALU operation)
    const ALUControl aluctl = ALUControl::decode(ctrl.aluOp(), instr);
    const uint8_t aluSel = aluctl.aluCtrl();

    // Register reads with one-instruction transparent bypass (from previous WB)
    uint32_t rs1Val = 0, rs2Val = 0;
    rf_.readPair(rs1, rs2, rs1Val, rs2Val, wb_.en, wb_.rd, wb_.data);

    // ALU compute (ALUSrc/LUISrc handled in ALU::decode)
    const ALU alu = ALU::decode(rs1Val, rs2Val, imm, aluSel, ctrl.aluSrc(), ctrl.luiSrc());
    const uint32_t aluResult = alu.aluResult();
    const bool branchZero = alu.branchZero(); // zero flag for BEQ/BNE

    // Branch decision (flip sense for BNE using bneCheck)
    const bool isBranch = ctrl.branch();
    const bool bneCheck = aluctl.bneCheck();
    const bool branchTaken = isBranch && (branchZero ^ bneCheck);

    // Jump classification
    const bool isJal  = (opcode == 0b1101111);
    const bool isJalr = (opcode == 0b1100111);
    const bool jump   = ctrl.jump();

    // Will this instruction touch data memory?
    const bool touchesMem = ctrl.memRead() || ctrl.memWrite();

    // Data memory compute (read value + stage store). No memory writes in Phase A.
    DataMemResult dmemRes{};   // default = no store, no read data
    if (touchesMem) {
        const uint32_t memAddr = effAddrMask(aluResult); // mask ONLY effective address
        dmemRes = DataMem::compute(
            memAddr,
            rs2Val,
            ctrl.memWrite(),
            ctrl.memRead(),
            ctrl.memToReg(),
            dmem_.data(), MEM_BYTES,
            funct3, funct3
        );
        // Stage the store for Phase B
        stagedStore_ = dmemRes.store();
    } else {
        // No memory action this instruction
        stagedStore_.clear();
    }

    // Select write-back data (JAL/JALR write PC+4; loads take memory; else ALU).
    uint32_t wbData = 0;
    if (jump && rd != 0) {
        wbData = pc_ + 4;
    } else if (ctrl.memToReg()) {
        wbData = dmemRes.writeData();
    } else {
        wbData = aluResult; // ALU / LUI / AUIPC, etc.
    }

    // Determine if we will write a register this cycle (JAL/JALR imply write).
    bool wbEn = ctrl.regWrite() || jump;

    // Next PC (still not committed)
    const uint32_t nextPC = computeNextPC(pc_, jump, branchTaken, imm, rs1Val, isJalr);

    // ===== Phase B: commit (apply side-effects) =====
    // Commit store to memory (if any)
    DataMem::commitStore(stagedStore_, dmem_.data(), MEM_BYTES);
    stagedStore_.clear();

    // Commit register write + update WB latch for next-cycle bypass
    if (wbEn && rd != 0) {
        rf_.commitWrite(rd, wbData);
        wb_.en   = true;
        wb_.rd   = rd;
        wb_.data = wbData;
    } else {
        wb_.en = false;
        wb_.rd = 0;
        wb_.data = 0;
    }

    // Update PC at end of cycle
    pc_ = nextPC;

    // Continue while next fetch is in range
    return (pc_ + 3 < MEM_BYTES);
}
