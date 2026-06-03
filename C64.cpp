#include "Common.h"
#include "C64.h"


void C64::cycle(){
  if (!cpuInCycles){
    cpu.cycle();
  }


}
