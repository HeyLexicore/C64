#include "Common.h"
#include "MOS6502.h"
#include "BUS.h"
#include "Debug.h"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

void MOS6502::getParams(OPERAND &op){
  switch (op.len) {
    case 3:
      J = bus->read8(PC+1);
      K = bus->read8(PC+2);
      break;
    case 2:
      J = bus->read8(PC+1);
      //printf("%02X\n",J);
    case 1:
    default:
      break;
  };

};


#define setT_ uint8_t T_ = getValue(mode)

uint16_t MOS6502::getAddress(AddrMode m){


  switch (m){
    case AddrMode::Implied:
      if (debug->warnModes) printf("Uhhh address of impl\n");
      break;
    case AddrMode::Accumulator:
      if (debug->warnModes) printf("Uhhh address of acc\n");
      break;
    case AddrMode::Immediate:
      if (debug->warnModes) printf("Uhhh address of imm\n");
      break;
    case AddrMode::Absolute:
      return J | ((uint16_t)K << 8);
    case AddrMode::AbsoluteX:
      return (J | ((uint16_t)K << 8)) + X;
    case AddrMode::AbsoluteY:
      return (J | ((uint16_t)K << 8)) + Y;
    case AddrMode::Indirect:
      return bus->read8(J | ((uint16_t)K << 8)) | ((uint16_t)bus->read8((J | ((uint16_t)K << 8)) + 1) << 8);
    case AddrMode::ZeroPage:
      return J;
    case AddrMode::ZeroPageX:
      return J+X;
    case AddrMode::ZeroPageY:
      return J+Y;
    case AddrMode::IndirectX:
      return  bus->read8(J+X) | (bus->read8(J+X+1) << 8);
    case AddrMode::IndirectY:
      return uint16_t(bus->read8(J)) + ((uint16_t)bus->read8(J+1) << 8) + Y;
    case AddrMode::Relative:
      return PC+static_cast<int8_t>(J);


  }
  return 0;
}

uint8_t MOS6502::getValue(AddrMode m){

  switch (m){
    case AddrMode::Implied:

      if (debug->warnModes) printf("Uhhh value of impl\n");
      return 0;
    case AddrMode::Accumulator:
      return A;
    case AddrMode::Immediate:
      return J;
    default:
      return bus->read8(getAddress(m));
  }
}

inline void MOS6502::setNZ(uint8_t val){
  P.N = ((val & 128) > 0);
  P.Z = (val == 0);
}

int MOS6502::cycle(){
  OPERAND op = fetch();
  getParams(op);

  if (debug->opcodes){
    printf("%s%s%s%s%s%s%s%s",
          P.N?"N":"-",
          P.V?"V":"-",
          P.EX?"-":"-",
          P.B?"B":"-",
          P.D?"D":"-",
          P.I?"I":"-",
          P.Z?"Z":"-",
          P.C?"C":"-");
    printf(" ");
    printf("\x1B[0;34m%04X \x1B[35m%04X \x1B[36mA %02X \x1B[32mX %02X \x1B[31mY %02X \x1B[34m%02X ",
                        SP,          PC,             A,             X,              Y,  op.opcode);
    
    switch (op.len){
      case 3:
        printf(op.mnemonic.c_str(),K,J); break;
      case 2:
        printf(op.mnemonic.c_str(),J);   break;
      case 1:
        printf(op.mnemonic.c_str(),NULL);break;
    }
    printf("\x1B[0m\n");
  }

  PC += op.len;
  if (op.function) (this->*op.function)(op.mode);
  return op.cycles;
}

MOS6502::OPERAND MOS6502::fetch(){
  return operandHolder[bus->read8(PC)];
}

void MOS6502::jump(bool jump){
  if (jump) {
    AddrMode mode = AddrMode::Relative;
    PC = getAddress(mode);
  } 
};

void MOS6502::ADC(AddrMode mode){
  if (P.D){
    uint16_t sum = uint16_t(A)+uint16_t(getValue(mode))+P.C;
    if ((sum & 0x0F) > 9) sum += 0x06;    // fix low nibble
    if ((sum & 0xF0) > 0x90) sum += 0x60;  // fix high nibble
    P.C = sum > 0xFF;
    A = uint8_t(sum);
    setNZ(A);


  }else{
    setT_;
    uint16_t sum = uint16_t(A)+uint16_t(T_)+P.C;
    P.C = (sum > 0xFF);
    A = sum & 0xFF;
    setNZ(sum);
    P.V = ~(A ^ T_) & (A ^ uint8_t(sum)) & 0x80;
  }
};
void MOS6502::AND(AddrMode mode) { A = A & getValue(mode); setNZ(A); };
void MOS6502::ASL(AddrMode mode) {
  uint8_t T_ = getValue(mode);
  P.C = (T_ & 0x80) != 0; 
  uint8_t T = T_ << 1;    
  if (mode == AddrMode::Accumulator){
    A = T;
  }else{
    bus->write8(getValue(mode), T);
  }
  setNZ(T);                  
};

void MOS6502::BCC(AddrMode mode) {jump(!P.C);};
void MOS6502::BCS(AddrMode mode) {jump(P.C);};
void MOS6502::BEQ(AddrMode mode) {jump(P.Z);};

void MOS6502::BIT(AddrMode mode){
  uint8_t val = getValue(mode);
  uint8_t result = A & val;
  P.Z = (result == 0);
  P.N = (val & 0x80);
  P.V = (val & 0x40);
};

void MOS6502::BMI(AddrMode mode) {jump(P.N);};
void MOS6502::BNE(AddrMode mode) {jump(!P.Z);};
void MOS6502::BPL(AddrMode mode) {jump(!P.N);};

void MOS6502::BRK(AddrMode mode) {
  throw std::runtime_error("BRK)");
  //uint16_t pcToPush = PC + 1;
  //stackPush((pcToPush >> 8) & 0xFF);
  //stackPush(pcToPush & 0xFF);
  //stackPush(P.pack() | 0x30); 
  //P.I = true;
  //uint8_t low = bus->read8(0xFFFE);
  //uint8_t high = bus->read8(0xFFFF);
  //PC = (high << 8) | low;
};

void MOS6502::BVC(AddrMode mode) {jump(!P.V);};
void MOS6502::BVS(AddrMode mode) {jump(P.V);};

void MOS6502::CMP(AddrMode mode) {
  uint8_t T_ = getValue(mode);
  uint16_t T = uint16_t(A) - uint16_t(T_);
  P.C = (A >= T_);
  setNZ(T);
};
void MOS6502::CPX(AddrMode mode){setT_; uint8_t T = X-T_; setNZ(T); P.C = (T >= T_);}; 
void MOS6502::CPY(AddrMode mode){setT_; uint8_t T = Y-T_; setNZ(T); P.C = (T >= T_);}; 
void MOS6502::DEC(AddrMode mode){setT_; bus->write8(getAddress(mode), T_); setNZ(T_);};
void MOS6502::DEX(AddrMode mode){X--; setNZ(X);};
void MOS6502::DEY(AddrMode mode){Y--; setNZ(Y);};
void MOS6502::EOR(AddrMode mode){A = A xor getValue(mode); setNZ(A);};
void MOS6502::INC(AddrMode mode){setT_; bus->write8(getAddress(mode), T_+1); setNZ(T_+1); };
void MOS6502::INX(AddrMode mode){X++; setNZ(X);};
void MOS6502::INY(AddrMode mode){Y++; setNZ(Y);};
void MOS6502::JMP(AddrMode mode){
  if (mode == AddrMode::Absolute){
    PC = J | (uint16_t)K << 8;
  }else if (mode == AddrMode::Indirect){
    uint16_t addr = J | (uint16_t)K << 8;
    uint8_t M, N;
    M = bus->read8(addr);
    N = bus->read8(addr+1);
    debug->raiseDump = true;
    printf("Jumping indirectly address %04X to %04X\n", addr,  M | (uint16_t)N << 8);
    PC = M | (uint16_t)N << 8;
  }
};
void MOS6502::JSR(AddrMode mode){ 
  stackPush(uint8_t((PC & 0xFF00) >> 8));
  stackPush(uint8_t(PC & 0xFF));
  PC = getAddress(mode);
};

void MOS6502::LDA(AddrMode mode) { A = getValue(mode); setNZ(A); };
void MOS6502::LDX(AddrMode mode) { X = getValue(mode); setNZ(X); };
void MOS6502::LDY(AddrMode mode) { Y = getValue(mode); setNZ(Y); };

void MOS6502::LSR(AddrMode mode) {
  
  setT_;
  P.C = T_ & 0x01;
  uint8_t T = T_ >> 1;
  if (mode == AddrMode::Accumulator){
    A = T;
  }else{
    bus->write8(getAddress(mode), T);
  }
  setNZ(T);
  P.N = 0;
};
void MOS6502::NOP(AddrMode mode) { return; };
void MOS6502::ORA(AddrMode mode) {A = A | getValue(mode); setNZ(A);};

void MOS6502::stackPush(uint8_t value){
  bus->write8(uint16_t(SP) + 0x0100, value);
  SP--;
};

uint8_t MOS6502::stackPull(){
  SP++;
  return bus->read8(uint16_t(SP) + 0x0100);
};


void MOS6502::PHA(AddrMode mode) { stackPush(A); };
void MOS6502::PHP(AddrMode mode) { stackPush(P.pack()); };
void MOS6502::PLA(AddrMode mode) { A = stackPull(); };
void MOS6502::PLP(AddrMode mode) { P.unpack(stackPull()); };

void MOS6502::ROL(AddrMode mode) {
  bool oldC = P.C;
  setT_;
  P.C = 0b10000000 & T_;
  uint8_t T = (T_ << 1) | P.C;
  if (mode == AddrMode::Accumulator){
    A = T;
  }else{
    bus->write8(getAddress(mode), T);
  }
  setNZ(T);
};

void MOS6502::ROR(AddrMode mode) {
  setT_;
  uint8_t oldCarry = P.C;
  P.C = (T_ & 0x01) != 0;
  uint8_t T = (T_ >> 1) | (oldCarry << 7);
  if(mode == AddrMode::Accumulator){
    A = T;
  }else{
    bus->write8(getAddress(mode), T);
  }

  setNZ(T);
};

void MOS6502::RTI(AddrMode mode) {
  PLP(AddrMode::Immediate);
  PC = uint16_t(stackPull()) | (uint16_t(stackPull()) << 8);
};
void MOS6502::RTS(AddrMode mode) {PC = uint16_t(stackPull()) | (uint16_t(stackPull()) << 8);};
void MOS6502::SBC(AddrMode mode) {
  setT_;
  uint8_t val = T_;
  if (P.D){
    uint16_t diff = uint16_t(A) - uint16_t(val) - (P.C ? 0 : 1);

    uint16_t al = (A & 0x0F) - (val & 0x0F) - (P.C ? 0 : 1);
    uint16_t ah = (A >> 4) - (val >> 4);

    if (al & 0x10) {
      al -= 6;
      ah--;
    }

    if (ah & 0x10) {
      ah -= 6;
      P.C = 0; 
    } else {
      P.C = 1;
    }

    A = uint8_t(((ah << 4) | (al & 0x0F)) & 0xFF);
    setNZ(A);

    P.V = ((A ^ val) & (A ^ uint8_t(diff)) & 0x80);
  }
  else{
    uint16_t diff = uint16_t(A) - uint16_t(val) - (P.C ? 0 : 1);

    P.C = !(diff & 0x100);  
    A = uint8_t(diff);
    setNZ(A);

    P.V = (A ^ val) & (A ^ uint8_t(diff)) & 0x80;
  }
}


void MOS6502::SEC(AddrMode mode) { P.C = 1; };
void MOS6502::SED(AddrMode mode) { P.D = 1; };
void MOS6502::SEI(AddrMode mode) { P.I = 1; };
void MOS6502::CLC(AddrMode mode) { P.C = 0; };
void MOS6502::CLD(AddrMode mode) { P.D = 0; };
void MOS6502::CLI(AddrMode mode) { P.I = 0; };
void MOS6502::CLV(AddrMode mode) { P.V = 0; };

void MOS6502::STA(AddrMode mode) { bus->write8(getAddress(mode), A); };
void MOS6502::STX(AddrMode mode) { bus->write8(getAddress(mode), X); };
void MOS6502::STY(AddrMode mode) { bus->write8(getAddress(mode), Y); };

void MOS6502::TAX(AddrMode mode) { X = A;  };
void MOS6502::TAY(AddrMode mode) { Y = A;  };
void MOS6502::TSX(AddrMode mode) { X = SP; };
void MOS6502::TXA(AddrMode mode) { A = X;  };
void MOS6502::TXS(AddrMode mode) { SP = X; };
void MOS6502::TYA(AddrMode mode) { A = Y;  };

