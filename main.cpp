#include "Common.h"
#include "C64.h"

int main (int argc, char *argv[]) {
  
  C64 c64;
  c64.init(argv[1], argv[2], argv[3]);
  c64.debug.opcodes = false;
  c64.debug.display = true;
  //c64.debug.warnModes = true;
  //c64.debug.unImplRegions = true;
  //c64.init(argv[1], argv[2], argv[3]);

  c64.dumpMem("out.dump");
  while (true){
    
    c64.cycle();
    if (c64.debug.raiseDump){
      c64.dumpMem("out.dump");
      c64.debug.raiseDump = false;
    }

    

  }


  return 0;
}
