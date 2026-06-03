#pragma once
#include "Common.h"
#include <cstdint>

class BUS{
  public:
    BUS(){};


    void write8(uint16_t addr, uint8_t val, bool touch = true){RAM[addr]=val;};
    uint8_t read8(uint16_t addr, bool touch = true){return RAM[addr];};

  private:
    uint8_t RAM[0x10000];
};
