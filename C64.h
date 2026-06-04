#pragma once
#include "BUS.h"
#include "MOS6502.h"
#include "Common.h"
#include <cstdint>

class C64{
  public:
    C64() {};

    void init(std::string kernal, std::string basic, std::string char_ram);

    void cycle();

    Debug debug;
  
    void dumpMem(std::string name) { bus.dumpMem(name); };
    void display();

    float getTime_ms();
  private:

    
    uint64_t cycles = 0;
    int cpuInCycles = 0;

    Bus bus{&debug};
    MOS6502 cpu{&bus, &debug};
    //CIA6526 cia1;
    //CIA6526 cia2;

};
