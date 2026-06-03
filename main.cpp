#include "Common.h"
#include "C64.h"

int main (int argc, char *argv[]) {
  
  C64 c64;
  c64.debug.opcodes = true;

  //c64.init(argv[1], argv[2], argv[3]);


  while (true){
    c64.cycle();
  }


  return 0;
}
