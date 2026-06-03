#pragma once
#include "BUS.h"
#include "MOS6502.h"
#include "Common.h"
#include <cstdint>

class C64{
  public:
    C64() = default;

    void init(std::string kernal, std::string basic, std::string char_ram);

    void cycle();

    Debug debug;
  private:


    uint64_t cycles = 0;
    int cpuInCycles = 0;

    BUS bus;
    MOS6502 cpu{&bus, &debug};
    //CIA6526 cia1;
    //CIA6526 cia2;

};
