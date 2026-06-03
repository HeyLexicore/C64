#include "Common.h"
#include "MOS6502.h"
#include "Debug.h"
#include <cstddef>
#include <cstdio>

void MOS6502::getParams(OPERAND &op){
  switch (op.len) {
    case 3:
      J = bus->read8(PC+1);
      K = bus->read8(PC+2);
      break;
    case 2:
      J = bus->read8(PC+1);
    case 1:
    default:
      break;
  };

};
uint16_t MOS6502::getAddress(OPERAND &op){


  switch (op.mode){
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
      return J | ((uint16_t)K << 8) + X;
    case AddrMode::AbsoluteY:
      return J | ((uint16_t)K << 8) + Y;
    case AddrMode::Indirect:
      return bus->read8(J | ((uint16_t)K << 8)) | ((uint16_t)bus->read8(J | ((uint16_t)K << 8) + 1) << 8);
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
        printf(op.mnemonic.c_str(),J,K); break;
      case 2:
        printf(op.mnemonic.c_str(),J);   break;
      case 1:
        printf(op.mnemonic.c_str(),NULL);break;
    }
    printf("\n");
  }

  PC += op.len;
  (this->*op.function)(op.mode);
  return op.cycles;
}

MOS6502::OPERAND MOS6502::fetch(){
  return operandHolder[bus->read8(PC)];
}


void MOS6502::ADC(AddrMode mode) { return; };
void MOS6502::AND(AddrMode mode) { return; };
void MOS6502::ASL(AddrMode mode) { return; };
void MOS6502::BCC(AddrMode mode) { return; };
void MOS6502::BCS(AddrMode mode) { return; };
void MOS6502::BEQ(AddrMode mode) { return; };
void MOS6502::BIT(AddrMode mode) { return; };
void MOS6502::BMI(AddrMode mode) { return; };
void MOS6502::BNE(AddrMode mode) { return; };
void MOS6502::BPL(AddrMode mode) { return; };
void MOS6502::BRK(AddrMode mode) { return; };
void MOS6502::BVC(AddrMode mode) { return; };
void MOS6502::BVS(AddrMode mode) { return; };
void MOS6502::CLC(AddrMode mode) { return; };
void MOS6502::CLD(AddrMode mode) { return; };
void MOS6502::CLI(AddrMode mode) { return; };
void MOS6502::CLV(AddrMode mode) { return; };
void MOS6502::CMP(AddrMode mode) { return; };
void MOS6502::CPX(AddrMode mode) { return; };
void MOS6502::CPY(AddrMode mode) { return; };
void MOS6502::DEC(AddrMode mode) { return; };
void MOS6502::DEX(AddrMode mode) { return; };
void MOS6502::DEY(AddrMode mode) { return; };
void MOS6502::EOR(AddrMode mode) { return; };
void MOS6502::INC(AddrMode mode) { return; };
void MOS6502::INX(AddrMode mode) { return; };
void MOS6502::INY(AddrMode mode) { return; };
void MOS6502::JMP(AddrMode mode) { return; };
void MOS6502::JSR(AddrMode mode) { return; };
void MOS6502::LDA(AddrMode mode) { return; };
void MOS6502::LDX(AddrMode mode) { return; };
void MOS6502::LDY(AddrMode mode) { return; };
void MOS6502::LSR(AddrMode mode) { return; };
void MOS6502::NOP(AddrMode mode) { return; };
void MOS6502::ORA(AddrMode mode) { return; };
void MOS6502::PHA(AddrMode mode) { return; };
void MOS6502::PHP(AddrMode mode) { return; };
void MOS6502::PLA(AddrMode mode) { return; };
void MOS6502::PLP(AddrMode mode) { return; };
void MOS6502::ROL(AddrMode mode) { return; };
void MOS6502::ROR(AddrMode mode) { return; };
void MOS6502::RTI(AddrMode mode) { return; };
void MOS6502::RTS(AddrMode mode) { return; };
void MOS6502::SBC(AddrMode mode) { return; };
void MOS6502::SEC(AddrMode mode) { return; };
void MOS6502::SED(AddrMode mode) { return; };
void MOS6502::SEI(AddrMode mode) { return; };
void MOS6502::STA(AddrMode mode) { return; };
void MOS6502::STX(AddrMode mode) { return; };
void MOS6502::STY(AddrMode mode) { return; };
void MOS6502::TAX(AddrMode mode) { return; };
void MOS6502::TAY(AddrMode mode) { return; };
void MOS6502::TSX(AddrMode mode) { return; };
void MOS6502::TXA(AddrMode mode) { return; };
void MOS6502::TXS(AddrMode mode) { return; };
void MOS6502::TYA(AddrMode mode) { return; };

