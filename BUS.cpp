#include "Common.h"
#include <cstdint>
#include <fstream>
#include "BUS.h"
#include "Debug.h"

void Bus::setup(){

  RAM[0x0000] = 0x2F; //Processor port data direction register
  RAM[0x0001] = 0x37; // Processor port
}


uint8_t Bus::read8(uint16_t addr){
  return read8(addr, false);
}

uint8_t Bus::read8(uint16_t addr, bool touch = true){
  //check for all the diffrent banking types
  bool kernalEN = (RAM[0x0001] & 0b010) >> 1;
  bool basicEN  = (RAM[0x0001] & 0b001);
  bool inoutEN  = (RAM[0x0001] & 0b100) >> 2;
  bool charEN   = (RAM[0x0001] & 0b111) == 3;
  bool allRAM   = (RAM[0x0001] & 0b111) == 0;

  switch (addr){
    case 0x0000 ... 0x9FFF:
      return RAM[addr];
    case 0xA000 ... 0xBFFF:
      if (basicEN){
        return BASIC[addr-0xA000];
      } else {
        return RAM[addr];
      }
    case 0xC000 ... 0xCFFF:
      return RAM[addr];
    case 0xD000 ... 0xDFFF:
      if (allRAM){
        return RAM[addr];
      } else if (inoutEN){

        switch (addr) {
          case 0xD000 ... 0xD3FF:
            //return vic.read8(addr-0xD000);
            if (debug->unImplRegions) printf("Read VIC at %04X\n",addr);
            return RAM[addr];
          case 0xD400 ... 0xD7FF:
            if (debug->unImplRegions) printf("Read SID at %04X\n",addr);
            return RAM[addr];
          case 0xD800 ... 0xDBFF:
            if (debug->unImplRegions) printf("Read COLOR at %04X\n",addr);
            return COLOR[addr-0xD800];
          case 0xDC00 ... 0xDCFF:
            if (debug->unImplRegions) printf("Read CIA1 at %04X\n",addr);
            return RAM[addr]; //cia1.read8(addr-0xDC00);
          case 0xDD00 ... 0xDDFF:
            if (debug->unImplRegions) printf("Read CIA2 at %04X\n",addr);
            return RAM[addr]; //cia2.read8(addr-0xDD00);
          case 0xDE00 ... 0xDEFF:
            if (debug->unImplRegions) printf("Read I/O1 at %04X\n",addr);
            return RAM[addr];
          case 0xDF00 ... 0xDFFF:
            if (debug->unImplRegions) printf("Read I/O2 at %04X\n",addr);
            return RAM[addr];


        }

      } else if (charEN) {
        return CHAR[addr-0xD000];
      }
    case 0xE000 ... 0xFFFF:
      if (kernalEN){
        return KERNAL[addr-0xE000];
      }else{
        return RAM[addr];
      }
    
  }

  return RAM[addr];
};



void Bus::write8(uint16_t addr, uint8_t val){
   
  bool kernalEN = (RAM[0x0001] & 0b010) >> 1;
  bool basicEN  = (RAM[0x0001] & 0b001);
  bool inoutEN  = (RAM[0x0001] & 0b100) >> 2;
  bool charEN   = (RAM[0x0001] & 0b111) == 3;
  bool allRAM   = (RAM[0x0001] & 0b111) == 0;
 

  switch (addr){
    case 0x0000 ... 0x9FFF:
      RAM[addr] = val;
      break;
    case 0xA000 ... 0xBFFF:
      if (basicEN){
        if (debug->writeProt) RED("Writing to Basic ROM\n");
      } else {
        RAM[addr] = val;
      }
      break;
    case 0xC000 ... 0xCFFF:
      RAM[addr] = val;
      break;
    case 0xD000 ... 0xDFFF:
      if (allRAM){
        RAM[addr] = val;
      break;
      } else if (inoutEN){

        switch (addr) {
          case 0xD000 ... 0xD3FF:
            //vic.write8(addr-0xD000,val);
            if (debug->unImplRegions) printf("Writing to VIC at %04X with %02X\n",addr,val);
            RAM[addr] = val;
            break;
          case 0xD400 ... 0xD7FF:
            if (debug->unImplRegions) printf("Writing SID at %04X with %02X\n",addr,val);
            RAM[addr] = val;
            break;
          case 0xD800 ... 0xDBFF:
            if (debug->unImplRegions) printf("Writing Color at %04X with %02X\n",addr,val);
            COLOR[addr-0xD800] = val;
            break;
          case 0xDC00 ... 0xDCFF:
            if (debug->unImplRegions) printf("Write CIA1  at %04X with %02X\n",addr,val);
            //cia1.write8(addr-0xDC00, val);
            RAM[addr] = val;
            break;
          case 0xDD00 ... 0xDDFF:
            if (debug->unImplRegions) printf("Write CIA2  at %04X with %02X\n",addr,val);
            //cia2.write8(addr-0xDD00, val);
            RAM[addr] = val;
            break;

          case 0xDE00 ... 0xDEFF:
            if (debug->unImplRegions) printf("Write I/O1 at %04X with %02X\n",addr,val);
            RAM[addr] = val;
            break;
          case 0xDF00 ... 0xDFFF:
            if (debug->unImplRegions) printf("Write I/O2 at %04X with %02X\n",addr,val);
            RAM[addr] = val;
            break;
        }
        break;


      } else if (charEN) {
        if (debug->unImplRegions) RED("Writing to Char ROM\n");
        break;
        
      }
    case 0xE000 ... 0xFFFF:
      if (kernalEN){
        if (debug->unImplRegions) RED("Writing to Kernal ROM\n");
        break;
      }else{
        RAM[addr] = val;
        break;
      }
    
  }

  RAM[addr] = val;
};

void Bus::writeBasic(std::string name){
    
  std::ifstream file(name, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char*>(&BASIC), size); 
  file.close();

}


void Bus::writeKernal(std::string name){
  std::ifstream file(name, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char*>(&KERNAL), size); 
  file.close();

}

char petsciiToAscii(uint8_t pet) {
  if (pet >= 0x20 && pet <= 0x3F) return (char)pet;
  if (pet >= 0x41 && pet <= 0x5A) return (char)pet;
  if (pet >= 0xC1 && pet <= 0xDA) return (char)(pet - 0x60);
  if (pet >= 0x61 && pet <= 0x7A) return (char)(pet - 0x20);
  return '.';
}

void Bus::dumpMem(std::string filename) {
  FILE* fp = fopen(filename.c_str(), "w");
  if (!fp) return;

  fprintf(fp, "--- C64 MEMORY DUMP ---\n");

  for (uint32_t addr = 0; addr <= 0xFFFF; addr += 16) {
    fprintf(fp, "$%04X: ", addr);

    // Hex
    for (int i = 0; i < 16; i++) {
      uint8_t byte = read8((uint16_t)(addr + i),false);
      fprintf(fp, "%02X ", byte);
      if (i == 7) fprintf(fp, " ");
    }

    fprintf(fp, " |");

    // PETSCII
    for (int i = 0; i < 16; i++) {
      uint8_t byte = read8((uint16_t)(addr + i),false);
      fprintf(fp, "%c", petsciiToAscii(byte));
    }

    fprintf(fp, "|\n");
  }

  fclose(fp);
}

