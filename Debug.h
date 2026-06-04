#pragma once
#include "Common.h"
#include <cstdio>

struct Debug{
  bool warnModes = false; // warn if program wants the address of implied accumulator or immediate 
  bool opcodes = false;
  bool unImplRegions = false;
  bool writeProt = false;

  bool raiseDump = false;
  bool display = false;  
};


void RED(std::string format);

void BLUE(std::string format);

