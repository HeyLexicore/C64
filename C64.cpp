#include "Common.h"
#include <iostream>
#include "C64.h"


void C64::cycle(){
  bus.write8(0xD012, 0); 

  if (!cpuInCycles){
    cpuInCycles = cpu.cycle();
  }
  cpuInCycles--;

 if (((int)getTime_ms())%16 == 0 && debug.display) display();

  cycles++;
}

void C64::init(std::string kernal, std::string basic, std::string char_ram){
  bus.setup();
  bus.writeKernal(kernal);
  bus.writeBasic (basic);

  cpu.setup();
};


std::string c64_to_ansi_bg[16] = {
    "\x1b[48;5;16m",  // 0: Black
    "\x1b[48;5;231m", // 1: White
    "\x1b[48;5;88m",  // 2: Red
    "\x1b[48;5;73m",  // 3: Cyan
    "\x1b[48;5;54m",  // 4: Purple
    "\x1b[48;5;71m",  // 5: Green
    "\x1b[48;5;18m",  // 6: Blue
    "\x1b[48;5;226m", // 7: Yellow
    "\x1b[48;5;130m", // 8: Orange
    "\x1b[48;5;52m",  // 9: Brown
    "\x1b[48;5;203m", // 10: Light Red
    "\x1b[48;5;240m", // 11: Dark Grey
    "\x1b[48;5;244m", // 12: Grey
    "\x1b[48;5;120m", // 13: Light Green
    "\x1b[48;5;117m", // 14: Light Blue
    "\x1b[48;5;250m"  // 15: Light Grey
};


uint8_t screenCodeToPetscii(uint8_t code) {
  if (code <= 31)  return code + 64;  // @, A-Z
  if (code <= 63)  return code;       // Spc, !, 0-9, etc.
  if (code <= 95)  return code + 64;  // Graphics/Lowercase
  if (code <= 127) return code + 64;  // More Graphics
  
  // Codes 128-255 are just "Reversed" versions of 0-127
  if (code >= 128) return screenCodeToPetscii(code - 128);
  
  return code;
};


void C64::display() {
  const uint16_t SCREEN_RAM_START = 0x0400;
  const uint16_t COLOR_RAM_START  = 0xD800;
  const uint16_t BG_COLOR_REG     = 0xD021;
  const int ROWS = 25;
  const int COLS = 40;

  std::string fg_colors[16] = {
    "\x1b[38;5;16m", "\x1b[38;5;231m", "\x1b[38;5;88m", "\x1b[38;5;73m",
    "\x1b[38;5;54m", "\x1b[38;5;71m", "\x1b[38;5;18m", "\x1b[38;5;226m",
    "\x1b[38;5;130m", "\x1b[38;5;52m", "\x1b[38;5;203m", "\x1b[38;5;240m",
    "\x1b[38;5;244m", "\x1b[38;5;120m", "\x1b[38;5;117m", "\x1b[38;5;250m"
  };

  std::string bg_colors[16] = {
    "\x1b[48;5;16m", "\x1b[48;5;231m", "\x1b[48;5;88m", "\x1b[48;5;73m",
    "\x1b[48;5;54m", "\x1b[48;5;71m", "\x1b[48;5;18m", "\x1b[48;5;226m",
    "\x1b[48;5;130m", "\x1b[48;5;52m", "\x1b[48;5;203m", "\x1b[48;5;240m",
    "\x1b[48;5;244m", "\x1b[48;5;120m", "\x1b[48;5;117m", "\x1b[48;5;250m"
  };

  uint8_t bgColorIdx= bus.read8(BG_COLOR_REG) & 0x0F;
  std::cout << "\x1b[0m\x1b" << "7\x1b[H\x1b[?25l";

  std::cout << bg_colors[bgColorIdx];
  //printf("\x1b[H"); 
  
  uint8_t lastFGIdx = 0;
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      uint16_t offset = (y * COLS) + x;
      
      uint8_t petVal = bus.RAM[SCREEN_RAM_START + offset];
      uint8_t fgColorIdx = bus.COLOR[offset] & 0x0F;

      if (fgColorIdx != lastFGIdx){
        //std::cout << fg_colors[fgColorIdx];
        lastFGIdx = fgColorIdx;
      }
      
      putchar(screenCodeToPetscii(petVal));
    }
    printf("\n");
  }

  printf("\x1b[0mCpu runtime: %i Real time runtime (ms): %i     \n", (int)cycles,  (int)getTime_ms());

  std::cout << "\x1b" << "8\x1b[?25h";
}

float C64::getTime_ms(){
  return cycles*1.0/1000.0;
}
