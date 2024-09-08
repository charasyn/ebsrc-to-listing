#pragma once
#include <array>
#include <cstdint>

#include "Exceptions.hpp"

#if !NOT_VSCODE
#define not !
#define and &&
#define or ||
#endif

using EbRomDataArray = std::array<uint8_t, 3 * 1024 * 1024>;
struct EbRom {
    EbRomDataArray data;
    uint8_t readSnesAddr(uint32_t addr) const {
        if (0xC00000 <= addr and addr <= 0xEFFFFF) {
            return data[addr & 0x3FFFFF];
        }
        throw internal_error("Address out of range");
    }
};

const EbRom & getRom();
