#pragma once
#include "Common.h"
#include "Debug.h"
#include <cstdint>


class Bus{
  public:
    uint8_t RAM[0x10000]; 
    uint8_t KERNAL[0x2000];
    uint8_t BASIC[0x2000];
    uint8_t CHAR[0x1000];
    uint8_t MEMIO[0x1000];
    uint8_t COLOR[0x0400];
    uint8_t tVal;
    
    Debug* debug;
    //Vic vic;
    //Cia1 cia1;
    //Cia2 cia2;

    void write8(uint16_t addr, uint8_t val);
    uint8_t read8(uint16_t addr, bool touch);
    uint8_t read8(uint16_t addr);

    void dumpMem(std::string filename);
    
    void writeKernal(std::string name);
    void writeBasic(std::string name);
    void setup();

    Bus(Debug *debug_) : debug(debug_) {};
    
};


