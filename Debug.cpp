#include "Common.h"
#include "Debug.h"

void RED(std::string format){
  printf(("\x1b[41m"+format).c_str(),NULL);
  printf("\033[49");
};

void BLUE(std::string format){
  printf(("\x1b[44m"+format).c_str(),NULL);
  printf("\033[49");
};

