#pragma once
#include <cstdint>
#include <cstddef>

struct DataStoreOp {
    bool     en{false};
    uint32_t addr{0};
    uint8_t  size{0};  // 1,2,4
    uint32_t data{0};  // little-endian payload
    void clear() { en = false; addr = 0; size = 0; data = 0; }
};

class DataMemResult {
public:
    uint32_t   writeData() const { return readData_; }
    DataStoreOp store()    const { return store_; }

    void setRead(uint32_t v)           { readData_ = v; }
    void setStore(const DataStoreOp& s){ store_ = s; }

private:
    uint32_t    readData_{0};
    DataStoreOp store_{};
};

class DataMem {
public:
    static DataMemResult compute(uint32_t addrMasked, uint32_t rs2Val,
                                 bool memWrite, bool memRead, bool memToReg,
                                 uint8_t* mem, std::size_t memBytes,
                                 uint8_t funct3Load, uint8_t funct3Store);

    static void commitStore(const DataStoreOp& s, uint8_t* mem, std::size_t memBytes);
};
