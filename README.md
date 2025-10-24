# RISC-V Single-Cycle CPU Simulator

A C++ implementation of a 32-bit **RISC-V single-cycle processor**, built from scratch to model the datapath and control logic of a basic RISC-V core.  
This project simulates instruction execution cycle-by-cycle, covering arithmetic, memory, and branch operations.

---

## 🧠 Overview

This simulator models a simplified **RV32I** processor and integrates all major datapath components:

- **Program Counter (PC)**
- **Instruction Memory**
- **Register File**
- **Immediate Generator**
- **ALU & ALU Control**
- **Main Control Unit**
- **Data Memory**

The CPU executes each instruction in one clock cycle, following the standard five-stage single-cycle datapath:
> **IF → ID → EX → MEM → WB**

---

## ⚙️ Supported Instructions

| Type | Instructions |
|------|---------------|
| **R-type** | `AND`, `OR`, `SUB`, `SRA` |
| **I-type** | `ADDI`, `ORI`, `SLTIU`, `LBU`, `LW` |
| **S-type** | `SH`, `SW` |
| **B-type** | `BNE` |
| **U-type** | `LUI` |
| **J-type** | `JALR` |

Control signals implemented include:

`RegWrite`, `ALUSrc`, `Branch`, `MemRead`, `MemWrite`, `MemToReg`, `ALUOp`, `LuiSrc`, and `Jump`.

---

## 🧩 Features

- Fully modular C++ class design (`CPU`, `ALU`, `ControlUnit`, etc.)
- Two-phase clock step (`compute` → `commit`)
- Hex-based instruction/data memory loading
- Detailed internal signal flow (ideal for educational tracing)
- Easily extendable for pipelining or hazard simulation

---

## 🛠️ Build & Run

```bash
g++ cpusim.cpp -o cpusim
./cpusim
