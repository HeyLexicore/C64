#pragma once
#include "BUS.h"
#include "Common.h"
#include <cstdint>



class MOS6502 {
  public:
    MOS6502 () {};
    MOS6502 (Bus* bus, Debug* debug) : bus(bus), debug(debug) {};
  

    void setup() { PC = ((uint16_t)bus->read8(0xFFFD) << 8) | bus->read8(0xFFFC); };

  private:
    Bus* bus;
    Debug* debug;

    uint16_t PC{};
    uint8_t A{}, X{}, Y{}, SP{0xFD};
    uint8_t J{}, K{};
      
    struct {
      bool C, Z, I, D, B, EX, V, N;
      uint8_t pack() const {
        return C | (Z << 1) | (I << 2) | (D << 3) | (B << 4) | (EX << 5) |
               (V << 6) | (N << 7);
      }
      void unpack(uint8_t v) {
        C = ((v & 0x1) != 0);
        Z = (((v >> 1) & 0x1) != 0);
        I = (((v >> 2) & 0x1) != 0);
        D = (((v >> 3) & 0x1) != 0);
        B = (((v >> 4) & 0x1) != 0);
        EX = (((v >> 5) & 0x1) != 0);
        V = (((v >> 6) & 0x1) != 0);
        N = (((v >> 7) & 0x1) != 0);
      }
    } P;
    
    void stackPush(uint8_t val);
    uint8_t stackPull();
    void jump(bool jump);

    bool IRQ = false;


    enum class AddrMode {
      Implied,              //Implied,
      Accumulator,          //Accumulator,
      Immediate,            //Immediate,
      Absolute,             //Absolute,
      AbsoluteX,            //X_Indexed_Absolute,
      AbsoluteY,            //Y_Indexed_Absolute,
      Indirect,             //Absolute_Indirect,
      ZeroPage,             //Zero_Page,
      ZeroPageX,            //X_Indexed_Zero_Page,
      ZeroPageY,            //Y_Indexed_Zero_Page,
      IndirectX,            //X_Indexed_Zero_Page_Indirect,
      IndirectY,            //Zero_Page_Indirect_Y_Indexed,
      Relative              //Relative
    };

    using CpuFunct = void (MOS6502::*)(AddrMode);

    class OPERAND{
      public:
        AddrMode mode;
        std::string mnemonic;
        int len;
        int cycles;
        CpuFunct function;
        uint8_t opcode;
         
        OPERAND (AddrMode mode, std::string mnemonic, int len, int cycles, CpuFunct function, uint8_t opcode) :
          mode(mode),
          mnemonic(mnemonic),
          len(len),
          cycles(cycles),
          function(function),
          opcode(opcode) {};
    };



    const OPERAND operandHolder[256] = {
  
 

    {AddrMode::Implied,     "BRK",           1, 7, &MOS6502::BRK, 0x00}, // Force Interrupt
    {AddrMode::IndirectX,   "ORA ($%02X,X)", 2, 6, &MOS6502::ORA, 0x01}, // ORA (zp,X)
    {AddrMode::Implied,     "KIL",           1, 2, nullptr      , 0x02}, // (undoc) JAM/KIL - locks CPU
    {AddrMode::IndirectX,   "SLO ($%02X,X)", 2, 8, nullptr      , 0x03}, // (undoc) ASL + ORA (zp,X)
    {AddrMode::ZeroPage,    "NOP $%02X",     2, 3, nullptr      , 0x04}, // (undoc) NOP zp
    {AddrMode::ZeroPage,    "ORA $%02X",     2, 3, &MOS6502::ORA, 0x05}, // ORA zp
    {AddrMode::ZeroPage,    "ASL $%02X",     2, 5, &MOS6502::ASL, 0x06}, // ASL zp
    {AddrMode::ZeroPage,    "SLO $%02X",     2, 5, nullptr      , 0x07}, // (undoc) ASL + ORA zp
    {AddrMode::Implied,     "PHP",           1, 3, &MOS6502::PHP, 0x08}, // Push Processor Status
    {AddrMode::Immediate,   "ORA #$%02X",    2, 2, &MOS6502::ORA, 0x09}, // ORA #$nn
    {AddrMode::Accumulator, "ASL",           1, 2, &MOS6502::ASL, 0x0A}, // ASL A
    {AddrMode::Immediate,   "ANC #$%02X",    2, 2, nullptr      , 0x0B}, // (undoc) AND + move bit7->Carry
    {AddrMode::Absolute,    "NOP %02X%02X", 3, 4, nullptr      , 0x0C}, // (undoc) NOP abs
    {AddrMode::Absolute,    "ORA %02X%02X",     3, 4, &MOS6502::ORA, 0x0D}, // ORA abs
    {AddrMode::Absolute,    "ASL %02X%02X",     3, 6, &MOS6502::ASL, 0x0E}, // ASL abs
    {AddrMode::Absolute,    "SLO %02X%02X",     3, 6, nullptr      , 0x0F}, // (undoc) ASL + ORA abs

    {AddrMode::Relative,    "BPL $%02X",           2, 2, &MOS6502::BPL, 0x10}, // Branch on Plus
    {AddrMode::IndirectY,   "ORA ($%02X),Y", 2, 5, &MOS6502::ORA, 0x11}, // ORA (zp),Y
    {AddrMode::Implied,     "KIL",           1, 2, nullptr      , 0x12}, // (undoc) JAM/KIL - locks CPU
    {AddrMode::IndirectY,   "SLO ($%02X),Y", 2, 8, nullptr      , 0x13}, // (undoc) ASL + ORA (zp),Y
    {AddrMode::ZeroPageX,   "NOP $%02X,X",   2, 4, nullptr      , 0x14}, // (undoc) NOP zp,X
    {AddrMode::ZeroPageX,   "ORA $%02X,X",   2, 4, &MOS6502::ORA, 0x15}, // ORA zp,X
    {AddrMode::ZeroPageX,   "ASL $%02X,X",   2, 6, &MOS6502::ASL, 0x16}, // ASL zp,X
    {AddrMode::ZeroPageX,   "SLO $%02X,X",   2, 6, nullptr      , 0x17}, // (undoc) ASL + ORA zp,X
    {AddrMode::Implied,     "CLC",           1, 2, &MOS6502::CLC, 0x18}, // Clear Carry
    {AddrMode::AbsoluteY,   "ORA %02X%02X,Y",   3, 4, &MOS6502::ORA, 0x19}, // ORA abs,Y
    {AddrMode::Implied,     "NOP",           1, 2, nullptr      , 0x1A}, // (undoc) NOP (1-byte)
    {AddrMode::AbsoluteY,   "SLO %02X%02X,Y",   3, 7, nullptr      , 0x1B}, // (undoc) ASL + ORA abs,Y
    {AddrMode::AbsoluteX,   "NOP %02X%02X,X",   3, 4, nullptr      , 0x1C}, // (undoc) NOP abs,X
    {AddrMode::AbsoluteX,   "ORA %02X%02X,X",   3, 4, &MOS6502::ORA, 0x1D}, // ORA abs,X
    {AddrMode::AbsoluteX,   "ASL %02X%02X,X",   3, 7, &MOS6502::ASL, 0x1E}, // ASL abs,X
    {AddrMode::AbsoluteX,   "SLO %02X%02X,X",   3, 7, nullptr      , 0x1F}, // (undoc) ASL + ORA abs,X

    {AddrMode::Absolute,    "JSR %02X%02X",     3, 6, &MOS6502::JSR, 0x20}, // Jump to Subroutine
    {AddrMode::IndirectX,   "AND ($%02X,X)", 2, 6, &MOS6502::AND, 0x21}, // AND (zp,X)
    {AddrMode::Implied,     "KIL",           1, 2, nullptr      , 0x22}, // (undoc) JAM/KIL - locks CPU
    {AddrMode::IndirectX,   "RLA ($%02X,X)", 2, 8, nullptr      , 0x23}, // (undoc) ROL + AND (zp,X)
    {AddrMode::ZeroPage,    "BIT $%02X",     2, 3, &MOS6502::BIT, 0x24}, // BIT zp
    {AddrMode::ZeroPage,    "AND $%02X",     2, 3, &MOS6502::AND, 0x25}, // AND zp
    {AddrMode::ZeroPage,    "ROL $%02X",     2, 5, &MOS6502::ROL, 0x26}, // ROL zp
    {AddrMode::ZeroPage,    "RLA $%02X",     2, 5, nullptr      , 0x27}, // (undoc) ROL + AND zp
    {AddrMode::Implied,     "PLP",           1, 4, &MOS6502::PLP, 0x28}, // Pull Processor Status
    {AddrMode::Immediate,   "AND #$%02X",    2, 2, &MOS6502::AND, 0x29}, // AND #$nn
    {AddrMode::Accumulator, "ROL",           1, 2, &MOS6502::ROL, 0x2A}, // ROL A
    {AddrMode::Immediate,   "ANC #$%02X",    2, 2, nullptr      , 0x2B}, // (undoc) AND + move bit7->Carry
    {AddrMode::Absolute,    "BIT %02X%02X",     3, 4, &MOS6502::BIT, 0x2C}, // BIT abs
    {AddrMode::Absolute,    "AND %02X%02X",     3, 4, &MOS6502::AND, 0x2D}, // AND abs
    {AddrMode::Absolute,    "ROL %02X%02X",     3, 6, &MOS6502::ROL, 0x2E}, // ROL abs
    {AddrMode::Absolute,    "RLA %02X%02X",     3, 6, nullptr      , 0x2F}, // (undoc) ROL + AND abs

    {AddrMode::Relative,    "BMI $%02X",           2, 2, &MOS6502::BMI, 0x30}, // Branch on Minus
    {AddrMode::IndirectY,   "AND ($%02X),Y", 2, 5, &MOS6502::AND, 0x31}, // AND (zp),Y
    {AddrMode::Implied,     "KIL",           1, 2, nullptr      , 0x32}, // (undoc) JAM/KIL - locks CPU
    {AddrMode::IndirectY,   "RLA ($%02X),Y", 2, 8, nullptr      , 0x33}, // (undoc) ROL + AND (zp),Y
    {AddrMode::ZeroPageX,   "NOP $%02X,X",   2, 4, nullptr      , 0x34}, // (undoc) NOP zp,X
    {AddrMode::ZeroPageX,   "AND $%02X,X",   2, 4, &MOS6502::AND, 0x35}, // AND zp,X
    {AddrMode::ZeroPageX,   "ROL $%02X,X",   2, 6, &MOS6502::ROL, 0x36}, // ROL zp,X
    {AddrMode::ZeroPageX,   "RLA $%02X,X",   2, 6, nullptr      , 0x37}, // (undoc) ROL + AND zp,X
    {AddrMode::Implied,     "SEC",           1, 2, &MOS6502::SEC, 0x38}, // Set Carry
    {AddrMode::AbsoluteY,   "AND %02X%02X,Y",   3, 4, &MOS6502::AND, 0x39}, // AND abs,Y
    {AddrMode::Implied,     "NOP",           1, 2, nullptr      , 0x3A}, // (undoc) NOP (1-byte)
    {AddrMode::AbsoluteY,   "RLA %02X%02X,Y",   3, 7, nullptr      , 0x3B}, // (undoc) ROL + AND abs,Y
    {AddrMode::AbsoluteX,   "NOP %02X%02X,X",   3, 4, nullptr      , 0x3C}, // (undoc) NOP abs,X
    {AddrMode::AbsoluteX,   "AND %02X%02X,X",   3, 4, &MOS6502::AND, 0x3D}, // AND abs,X
    {AddrMode::AbsoluteX,   "ROL %02X%02X,X",   3, 7, &MOS6502::ROL, 0x3E}, // ROL abs,X
    {AddrMode::AbsoluteX,   "RLA %02X%02X,X",   3, 7, nullptr      , 0x3F}, // (undoc) ROL + AND abs,X

    {AddrMode::Implied,     "RTI",           1, 6, &MOS6502::RTI, 0x40}, // Return from Interrupt
    {AddrMode::IndirectX,   "EOR ($%02X,X)", 2, 6, &MOS6502::EOR, 0x41}, // EOR (zp,X)
    {AddrMode::Implied,     "KIL",           1, 2, nullptr      , 0x42}, // (undoc) JAM/KIL - locks CPU
    {AddrMode::IndirectX,   "SRE ($%02X,X)", 2, 8, nullptr      , 0x43}, // (undoc) LSR + EOR (zp,X)
    {AddrMode::ZeroPage,    "NOP $%02X",     2, 3, nullptr      , 0x44}, // (undoc) NOP zp
    {AddrMode::ZeroPage,    "EOR $%02X",     2, 3, &MOS6502::EOR, 0x45}, // EOR zp
    {AddrMode::ZeroPage,    "LSR $%02X",     2, 5, &MOS6502::LSR, 0x46}, // LSR zp
    {AddrMode::ZeroPage,    "SRE $%02X",     2, 5, nullptr      , 0x47}, // (undoc) LSR + EOR zp
    {AddrMode::Implied,     "PHA",           1, 3, &MOS6502::PHA, 0x48}, // Push Accumulator
    {AddrMode::Immediate,   "EOR #$%02X",    2, 2, &MOS6502::EOR, 0x49}, // EOR #$nn
    {AddrMode::Accumulator, "LSR",           1, 2, &MOS6502::LSR, 0x4A}, // LSR A
    {AddrMode::Immediate,   "ALR #$%02X",    2, 2, nullptr      , 0x4B}, // (undoc) AND + LSR
    {AddrMode::Absolute,    "JMP %02X%02X",     3, 3, &MOS6502::JMP, 0x4C}, // JMP abs
    {AddrMode::Absolute,    "EOR %02X%02X",     3, 4, &MOS6502::EOR, 0x4D}, // EOR abs
    {AddrMode::Absolute,    "LSR %02X%02X",     3, 6, &MOS6502::LSR, 0x4E}, // LSR abs
    {AddrMode::Absolute,    "SRE %02X%02X",     3, 6, nullptr      , 0x4F}, // (undoc) LSR + EOR abs

    {AddrMode::Relative,    "BVC $%02X",           2, 2, &MOS6502::BVC, 0x50}, // Branch if Overflow Clear
    {AddrMode::IndirectY,   "EOR ($%02X),Y", 2, 5, &MOS6502::EOR, 0x51}, // EOR (zp),Y
    {AddrMode::Implied,     "KIL",           1, 2, nullptr      , 0x52}, // (undoc) JAM/KIL - locks CPU
    {AddrMode::IndirectY,   "SRE ($%02X),Y", 2, 8, nullptr      , 0x53}, // (undoc) LSR + EOR (zp),Y
    {AddrMode::ZeroPageX,   "NOP $%02X,X",   2, 4, nullptr      , 0x54}, // (undoc) NOP zp,X
    {AddrMode::ZeroPageX,   "EOR $%02X,X",   2, 4, &MOS6502::EOR, 0x55}, // EOR zp,X
    {AddrMode::ZeroPageX,   "LSR $%02X,X",   2, 6, &MOS6502::LSR, 0x56}, // LSR zp,X
    {AddrMode::ZeroPageX,   "SRE $%02X,X",   2, 6, nullptr      , 0x57}, // (undoc) LSR + EOR zp,X
    {AddrMode::Implied,     "CLI",           1, 2, &MOS6502::CLI, 0x58}, // Clear Interrupt Disable
    {AddrMode::AbsoluteY,   "EOR %02X%02X,Y",   3, 4, &MOS6502::EOR, 0x59}, // EOR abs,Y
    {AddrMode::Implied,     "NOP",           1, 2, nullptr      , 0x5A}, // (undoc) NOP (1-byte)
    {AddrMode::AbsoluteY,   "SRE %02X%02X,Y",   3, 7, nullptr      , 0x5B}, // (undoc) LSR + EOR abs,Y
    {AddrMode::AbsoluteX,   "NOP %02X%02X,X",   3, 4, nullptr      , 0x5C}, // (undoc) NOP abs,X
    {AddrMode::AbsoluteX,   "EOR %02X%02X,X",   3, 4, &MOS6502::EOR, 0x5D}, // EOR abs,X
    {AddrMode::AbsoluteX,   "LSR %02X%02X,X",   3, 7, &MOS6502::LSR, 0x5E}, // LSR abs,X
    {AddrMode::AbsoluteX,   "SRE %02X%02X,X",   3, 7, nullptr      , 0x5F}, // (undoc) LSR + EOR abs,X

    {AddrMode::Implied,     "RTS",           1, 6, &MOS6502::RTS, 0x60}, // Return from Subroutine
    {AddrMode::IndirectX,   "ADC ($%02X,X)", 2, 6, &MOS6502::ADC, 0x61}, // ADC (zp,X)
    {AddrMode::Implied,     "KIL",           1, 2, nullptr      , 0x62}, // (undoc) JAM/KIL - locks CPU
    {AddrMode::IndirectX,   "RRA ($%02X,X)", 2, 8, nullptr      , 0x63}, // (undoc) ROR + ADC (zp,X)
    {AddrMode::ZeroPage,    "NOP $%02X",     2, 3, nullptr      , 0x64}, // (undoc) NOP zp
    {AddrMode::ZeroPage,    "ADC $%02X",     2, 3, &MOS6502::ADC, 0x65}, // ADC zp
    {AddrMode::ZeroPage,    "ROR $%02X",     2, 5, &MOS6502::ROR, 0x66}, // ROR zp
    {AddrMode::ZeroPage,    "RRA $%02X",     2, 5, nullptr      , 0x67}, // (undoc) ROR + ADC zp
    {AddrMode::Implied,     "PLA",           1, 4, &MOS6502::PLA, 0x68}, // Pull Accumulator
    {AddrMode::Immediate,   "ADC #$%02X",    2, 2, &MOS6502::ADC, 0x69}, // ADC #$nn
    {AddrMode::Accumulator, "ROR",           1, 2, &MOS6502::ROR, 0x6A}, // ROR A
    {AddrMode::Immediate,   "ARR #$%02X",    2, 2, nullptr      , 0x6B}, // (undoc) AND + ROR, special flags
    {AddrMode::Indirect,    "JMP ($%02X%02X)",   3, 5, &MOS6502::JMP, 0x6C}, // JMP (addr)
    {AddrMode::Absolute,    "ADC %02X%02X",     3, 4, &MOS6502::ADC, 0x6D}, // ADC abs
    {AddrMode::Absolute,    "ROR %02X%02X",     3, 6, &MOS6502::ROR, 0x6E}, // ROR abs
    {AddrMode::Absolute,    "RRA %02X%02X",     3, 6, nullptr      , 0x6F}, // (undoc) ROR + ADC abs

    {AddrMode::Relative,    "BVS $%02X",           2, 2, &MOS6502::BVS, 0x70}, // Branch if Overflow Set
    {AddrMode::IndirectY,   "ADC ($%02X),Y", 2, 5, &MOS6502::ADC, 0x71}, // ADC (zp),Y
    {AddrMode::Implied,     "KIL",           1, 2, nullptr      , 0x72}, // (undoc) JAM/KIL - locks CPU
    {AddrMode::IndirectY,   "RRA ($%02X),Y", 2, 8, nullptr      , 0x73}, // (undoc) ROR + ADC (zp),Y
    {AddrMode::ZeroPageX,   "NOP $%02X,X",   2, 4, nullptr      , 0x74}, // (undoc) NOP zp,X
    {AddrMode::ZeroPageX,   "ADC $%02X,X",   2, 4, &MOS6502::ADC, 0x75}, // ADC zp,X
    {AddrMode::ZeroPageX,   "ROR $%02X,X",   2, 6, &MOS6502::ROR, 0x76}, // ROR zp,X
    {AddrMode::ZeroPageX,   "RRA $%02X,X",   2, 6, nullptr      , 0x77}, // (undoc) ROR + ADC zp,X
    {AddrMode::Implied,     "SEI",           1, 2, &MOS6502::SEI, 0x78}, // Set Interrupt Disable
    {AddrMode::AbsoluteY,   "ADC %02X%02X,Y",   3, 4, &MOS6502::ADC, 0x79}, // ADC abs,Y
    {AddrMode::Implied,     "NOP",           1, 2, nullptr      , 0x7A}, // (undoc) NOP (1-byte)
    {AddrMode::AbsoluteY,   "RRA %02X%02X,Y",   3, 7, nullptr      , 0x7B}, // (undoc) ROR + ADC abs,Y
    {AddrMode::AbsoluteX,   "NOP %02X%02X,X",   3, 4, nullptr      , 0x7C}, // (undoc) NOP abs,X
    {AddrMode::AbsoluteX,   "ADC %02X%02X,X",   3, 4, &MOS6502::ADC, 0x7D}, // ADC abs,X
    {AddrMode::AbsoluteX,   "ROR %02X%02X,X",   3, 7, &MOS6502::ROR, 0x7E}, // ROR abs,X
    {AddrMode::AbsoluteX,   "RRA %02X%02X,X",   3, 7, nullptr      , 0x7F}, // (undoc) ROR + ADC abs,X

    {AddrMode::Immediate,   "NOP #$%02X",    2, 2, nullptr      , 0x80}, // (undoc) NOP #$nn
    {AddrMode::IndirectX,   "STA ($%02X,X)", 2, 6, &MOS6502::STA, 0x81}, // STA (zp,X)
    {AddrMode::Immediate,   "NOP #$%02X",    2, 2, nullptr      , 0x82}, // (undoc) NOP #$nn
    {AddrMode::IndirectX,   "SAX ($%02X,X)", 2, 6, nullptr      , 0x83}, // (undoc) Store A & X at (zp,X)
    {AddrMode::ZeroPage,    "STY $%02X",     2, 3, &MOS6502::STY, 0x84}, // STY zp
    {AddrMode::ZeroPage,    "STA $%02X",     2, 3, &MOS6502::STA, 0x85}, // STA zp
    {AddrMode::ZeroPage,    "STX $%02X",     2, 3, &MOS6502::STX, 0x86}, // STX zp
    {AddrMode::ZeroPage,    "SAX $%02X",     2, 3, nullptr      , 0x87}, // (undoc) Store A & X at zp
    {AddrMode::Implied,     "DEY",           1, 2, &MOS6502::DEY, 0x88}, // Decrement Y
    {AddrMode::Immediate,   "NOP #$%02X",    2, 2, nullptr      , 0x89}, // (undoc) NOP #$nn
    {AddrMode::Implied,     "TXA",           1, 2, &MOS6502::TXA, 0x8A}, // Transfer X to A
    {AddrMode::Immediate,   "XAA #$%02X",    2, 2, nullptr      , 0x8B}, // (undoc) Highly unstable AND
    {AddrMode::Absolute,    "STY %02X%02X",     3, 4, &MOS6502::STY, 0x8C}, // STY abs
    {AddrMode::Absolute,    "STA %02X%02X",     3, 4, &MOS6502::STA, 0x8D}, // STA abs
    {AddrMode::Absolute,    "STX %02X%02X",     3, 4, &MOS6502::STX, 0x8E}, // STX abs
    {AddrMode::Absolute,    "SAX %02X%02X",     3, 4, nullptr      , 0x8F}, // (undoc) Store A & X at abs

    {AddrMode::Relative,    "BCC $%02X",           2, 2, &MOS6502::BCC, 0x90}, // Branch if Carry Clear
    {AddrMode::IndirectY,   "STA ($%02X),Y", 2, 6, &MOS6502::STA, 0x91}, // STA (zp),Y
    {AddrMode::Implied,     "KIL",           1, 2, nullptr      , 0x92}, // (undoc) JAM/KIL - locks CPU
    {AddrMode::IndirectY,   "AHX ($%02X),Y", 2, 6, nullptr      , 0x93}, // (undoc) Store A & X & (high byte+1)
    {AddrMode::ZeroPageX,   "STY $%02X,X",   2, 4, &MOS6502::STY, 0x94}, // STY zp,X
    {AddrMode::ZeroPageX,   "STA $%02X,X",   2, 4, &MOS6502::STA, 0x95}, // STA zp,X
    {AddrMode::ZeroPageY,   "STX $%02X,Y",   2, 4, &MOS6502::STX, 0x96}, // STX zp,Y
    {AddrMode::ZeroPageY,   "SAX $%02X,Y",   2, 4, nullptr      , 0x97}, // (undoc) Store A & X at zp,Y
    {AddrMode::Implied,     "TYA",           1, 2, &MOS6502::TYA, 0x98}, // Transfer Y to A
    {AddrMode::AbsoluteY,   "STA %02X%02X,Y",   3, 5, &MOS6502::STA, 0x99}, // STA abs,Y
    {AddrMode::Implied,     "TXS",           1, 2, &MOS6502::TXS, 0x9A}, // Transfer X to Stack Ptr
    {AddrMode::AbsoluteY,   "TAS %02X%02X,Y",   3, 5, nullptr      , 0x9B}, // (undoc) Transfer A & X to SP, store
    {AddrMode::AbsoluteX,   "SHY %02X%02X,X",   3, 5, nullptr      , 0x9C}, // (undoc) Store Y & (high byte+1)
    {AddrMode::AbsoluteX,   "STA %02X%02X,X",   3, 5, &MOS6502::STA, 0x9D}, // STA abs,X
    {AddrMode::AbsoluteY,   "SHX %02X%02X,Y",   3, 5, nullptr      , 0x9E}, // (undoc) Store X & (high byte+1)
    {AddrMode::AbsoluteY,   "AHX %02X%02X,Y",   3, 5, nullptr      , 0x9F}, // (undoc) Store A & X & (high byte+1)

    {AddrMode::Immediate,   "LDY #$%02X",    2, 2, &MOS6502::LDY, 0xA0}, // LDY #$nn
    {AddrMode::IndirectX,   "LDA ($%02X,X)", 2, 6, &MOS6502::LDA, 0xA1}, // LDA (zp,X)
    {AddrMode::Immediate,   "LDX #$%02X",    2, 2, &MOS6502::LDX, 0xA2}, // LDX #$nn
    {AddrMode::IndirectX,   "LAX ($%02X,X)", 2, 6, nullptr      , 0xA3}, // (undoc) Load A & X from (zp,X)
    {AddrMode::ZeroPage,    "LDY $%02X",     2, 3, &MOS6502::LDY, 0xA4}, // LDY zp
    {AddrMode::ZeroPage,    "LDA $%02X",     2, 3, &MOS6502::LDA, 0xA5}, // LDA zp
    {AddrMode::ZeroPage,    "LDX $%02X",     2, 3, &MOS6502::LDX, 0xA6}, // LDX zp
    {AddrMode::ZeroPage,    "LAX $%02X",     2, 3, nullptr      , 0xA7}, // (undoc) Load A & X from zp
    {AddrMode::Implied,     "TAY",           1, 2, &MOS6502::TAY, 0xA8}, // Transfer A to Y
    {AddrMode::Immediate,   "LDA #$%02X",    2, 2, &MOS6502::LDA, 0xA9}, // LDA #$nn
    {AddrMode::Implied,     "TAX",           1, 2, &MOS6502::TAX, 0xAA}, // Transfer A to X
    {AddrMode::Immediate,   "LAX #$%02X",    2, 2, nullptr      , 0xAB}, // (undoc) Load A & X (unstable)
    {AddrMode::Absolute,    "LDY %02X%02X",     3, 4, &MOS6502::LDY, 0xAC}, // LDY abs
    {AddrMode::Absolute,    "LDA %02X%02X",     3, 4, &MOS6502::LDA, 0xAD}, // LDA abs
    {AddrMode::Absolute,    "LDX %02X%02X",     3, 4, &MOS6502::LDX, 0xAE}, // LDX abs
    {AddrMode::Absolute,    "LAX %02X%02X",     3, 4, nullptr      , 0xAF}, // (undoc) Load A & X from abs

    {AddrMode::Relative,    "BCS $%02X",           2, 2, &MOS6502::BCS, 0xB0}, // Branch if Carry Set
    {AddrMode::IndirectY,   "LDA ($%02X),Y", 2, 5, &MOS6502::LDA, 0xB1}, // LDA (zp),Y
    {AddrMode::Implied,     "KIL",           1, 2, nullptr      , 0xB2}, // (undoc) JAM/KIL - locks CPU
    {AddrMode::IndirectY,   "LAX ($%02X),Y", 2, 5, nullptr      , 0xB3}, // (undoc) Load A & X from (zp),Y
    {AddrMode::ZeroPageX,   "LDY $%02X,X",   2, 4, &MOS6502::LDY, 0xB4}, // LDY zp,X
    {AddrMode::ZeroPageX,   "LDA $%02X,X",   2, 4, &MOS6502::LDA, 0xB5}, // LDA zp,X
    {AddrMode::ZeroPageY,   "LDX $%02X,Y",   2, 4, &MOS6502::LDX, 0xB6}, // LDX zp,Y
    {AddrMode::ZeroPageY,   "LAX $%02X,Y",   2, 4, nullptr      , 0xB7}, // (undoc) Load A & X from zp,Y
    {AddrMode::Implied,     "CLV",           1, 2, &MOS6502::CLV, 0xB8}, // Clear Overflow
    {AddrMode::AbsoluteY,   "LDA %02X%02X,Y",   3, 4, &MOS6502::LDA, 0xB9}, // LDA abs,Y
    {AddrMode::Implied,     "TSX",           1, 2, &MOS6502::TSX, 0xBA}, // Transfer SP to X
    {AddrMode::AbsoluteY,   "LAS %02X%02X,Y",   3, 4, nullptr      , 0xBB}, // (undoc) Load A,X,SP = SP & M
    {AddrMode::AbsoluteX,   "LDY %02X%02X,X",   3, 4, &MOS6502::LDY, 0xBC}, // LDY abs,X
    {AddrMode::AbsoluteX,   "LDA %02X%02X,X",   3, 4, &MOS6502::LDA, 0xBD}, // LDA abs,X
    {AddrMode::AbsoluteY,   "LDX %02X%02X,Y",   3, 4, &MOS6502::LDX, 0xBE}, // LDX abs,Y
    {AddrMode::AbsoluteY,   "LAX %02X%02X,Y",   3, 4, nullptr      , 0xBF}, // (undoc) Load A & X from abs,Y

    {AddrMode::Immediate,   "CPY #$%02X",    2, 2, &MOS6502::CPY, 0xC0}, // CPY #$nn
    {AddrMode::IndirectX,   "CMP ($%02X,X)", 2, 6, &MOS6502::CMP, 0xC1}, // CMP (zp,X)
    {AddrMode::Immediate,   "NOP #$%02X",    2, 2, nullptr      , 0xC2}, // (undoc) NOP #$nn
    {AddrMode::IndirectX,   "DCP ($%02X,X)", 2, 8, nullptr      , 0xC3}, // (undoc) DEC + CMP (zp,X)
    {AddrMode::ZeroPage,    "CPY $%02X",     2, 3, &MOS6502::CPY, 0xC4}, // CPY zp
    {AddrMode::ZeroPage,    "CMP $%02X",     2, 3, &MOS6502::CMP, 0xC5}, // CMP zp
    {AddrMode::ZeroPage,    "DEC $%02X",     2, 5, &MOS6502::DEC, 0xC6}, // DEC zp
    {AddrMode::ZeroPage,    "DCP $%02X",     2, 5, nullptr      , 0xC7}, // (undoc) DEC + CMP zp
    {AddrMode::Implied,     "INY",           1, 2, &MOS6502::INY, 0xC8}, // Increment Y
    {AddrMode::Immediate,   "CMP #$%02X",    2, 2, &MOS6502::CMP, 0xC9}, // CMP #$nn
    {AddrMode::Implied,     "DEX",           1, 2, &MOS6502::DEX, 0xCA}, // Decrement X
    {AddrMode::Immediate,   "AXS #$%02X",    2, 2, nullptr      , 0xCB}, // (undoc) A & X -> X, compare with #$nn
    {AddrMode::Absolute,    "CPY %02X%02X",     3, 4, &MOS6502::CPY, 0xCC}, // CPY abs
    {AddrMode::Absolute,    "CMP %02X%02X",     3, 4, &MOS6502::CMP, 0xCD}, // CMP abs
    {AddrMode::Absolute,    "DEC %02X%02X",     3, 6, &MOS6502::DEC, 0xCE}, // DEC abs
    {AddrMode::Absolute,    "DCP %02X%02X",     3, 6, nullptr      , 0xCF}, // (undoc) DEC + CMP abs

    {AddrMode::Relative,    "BNE $%02X",           2, 2, &MOS6502::BNE, 0xD0}, // Branch if Not Equal
    {AddrMode::IndirectY,   "CMP ($%02X),Y", 2, 5, &MOS6502::CMP, 0xD1}, // CMP (zp),Y
    {AddrMode::Implied,     "KIL",           1, 2, nullptr      , 0xD2}, // (undoc) JAM/KIL - locks CPU
    {AddrMode::IndirectY,   "DCP ($%02X),Y", 2, 8, nullptr      , 0xD3}, // (undoc) DEC + CMP (zp),Y
    {AddrMode::ZeroPageX,   "NOP $%02X,X",   2, 4, nullptr      , 0xD4}, // (undoc) NOP zp,X
    {AddrMode::ZeroPageX,   "CMP $%02X,X",   2, 4, &MOS6502::CMP, 0xD5}, // CMP zp,X
    {AddrMode::ZeroPageX,   "DEC $%02X,X",   2, 6, &MOS6502::DEC, 0xD6}, // DEC zp,X
    {AddrMode::ZeroPageX,   "DCP $%02X,X",   2, 6, nullptr      , 0xD7}, // (undoc) DEC + CMP zp,X
    {AddrMode::Implied,     "CLD",           1, 2, &MOS6502::CLD, 0xD8}, // Clear Decimal
    {AddrMode::AbsoluteY,   "CMP %02X%02X,Y",   3, 4, &MOS6502::CMP, 0xD9}, // CMP abs,Y
    {AddrMode::Implied,     "NOP",           1, 2, nullptr      , 0xDA}, // (undoc) NOP (1-byte)
    {AddrMode::AbsoluteY,   "DCP %02X%02X,Y",   3, 7, nullptr      , 0xDB}, // (undoc) DEC + CMP abs,Y
    {AddrMode::AbsoluteX,   "NOP %02X%02X,X",   3, 4, nullptr      , 0xDC}, // (undoc) NOP abs,X
    {AddrMode::AbsoluteX,   "CMP %02X%02X,X",   3, 4, &MOS6502::CMP, 0xDD}, // CMP abs,X
    {AddrMode::AbsoluteX,   "DEC %02X%02X,X",   3, 7, &MOS6502::DEC, 0xDE}, // DEC abs,X
    {AddrMode::AbsoluteX,   "DCP %02X%02X,X",   3, 7, nullptr      , 0xDF}, // (undoc) DEC + CMP abs,X

    {AddrMode::Immediate,   "CPX #$%02X",    2, 2, &MOS6502::CPX, 0xE0}, // CPX #$nn
    {AddrMode::IndirectX,   "SBC ($%02X,X)", 2, 6, &MOS6502::SBC, 0xE1}, // SBC (zp,X)
    {AddrMode::Immediate,   "NOP #$%02X",    2, 2, nullptr      , 0xE2}, // (undoc) NOP #$nn
    {AddrMode::IndirectX,   "ISC ($%02X,X)", 2, 8, nullptr      , 0xE3}, // (undoc) INC + SBC (zp,X)
    {AddrMode::ZeroPage,    "CPX $%02X",     2, 3, &MOS6502::CPX, 0xE4}, // CPX zp
    {AddrMode::ZeroPage,    "SBC $%02X",     2, 3, &MOS6502::SBC, 0xE5}, // SBC zp
    {AddrMode::ZeroPage,    "INC $%02X",     2, 5, &MOS6502::INC, 0xE6}, // INC zp
    {AddrMode::ZeroPage,    "ISC $%02X",     2, 5, nullptr      , 0xE7}, // (undoc) INC + SBC zp
    {AddrMode::Implied,     "INX",           1, 2, &MOS6502::INX, 0xE8}, // Increment X
    {AddrMode::Immediate,   "SBC #$%02X",    2, 2, &MOS6502::SBC, 0xE9}, // SBC #$nn
    {AddrMode::Implied,     "NOP",           1, 2, &MOS6502::NOP, 0xEA}, // Official NOP
    {AddrMode::Immediate,   "SBC #$%02X",    2, 2, nullptr      , 0xEB}, // (undoc) SBC #$nn (alias of E9)
    {AddrMode::Absolute,    "CPX %02X%02X",     3, 4, &MOS6502::CPX, 0xEC}, // CPX abs
    {AddrMode::Absolute,    "SBC %02X%02X",     3, 4, &MOS6502::SBC, 0xED}, // SBC abs
    {AddrMode::Absolute,    "INC %02X%02X",     3, 6, &MOS6502::INC, 0xEE}, // INC abs
    {AddrMode::Absolute,    "ISC %02X%02X",     3, 6, nullptr      , 0xEF}, // (undoc) INC + SBC abs

    {AddrMode::Relative,    "BEQ $%02X",           2, 2, &MOS6502::BEQ, 0xF0}, // Branch if Equal
    {AddrMode::IndirectY,   "SBC ($%02X),Y", 2, 5, &MOS6502::SBC, 0xF1}, // SBC (zp),Y
    {AddrMode::Implied,     "KIL",           1, 2, nullptr      , 0xF2}, // (undoc) JAM/KIL - locks CPU
    {AddrMode::IndirectY,   "ISC ($%02X),Y", 2, 8, nullptr      , 0xF3}, // (undoc) INC + SBC (zp),Y
    {AddrMode::ZeroPageX,   "NOP $%02X,X",   2, 4, nullptr      , 0xF4}, // (undoc) NOP zp,X
    {AddrMode::ZeroPageX,   "SBC $%02X,X",   2, 4, &MOS6502::SBC, 0xF5}, // SBC zp,X
    {AddrMode::ZeroPageX,   "INC $%02X,X",   2, 6, &MOS6502::INC, 0xF6}, // INC zp,X
    {AddrMode::ZeroPageX,   "ISC $%02X,X",   2, 6, nullptr      , 0xF7}, // (undoc) INC + SBC zp,X
    {AddrMode::Implied,     "SED",           1, 2, &MOS6502::SED, 0xF8}, // Set Decimal
    {AddrMode::AbsoluteY,   "SBC %02X%02X,Y",   3, 4, &MOS6502::SBC, 0xF9}, // SBC abs,Y
    {AddrMode::Implied,     "NOP",           1, 2, nullptr      , 0xFA}, // (undoc) NOP (1-byte)
    {AddrMode::AbsoluteY,   "ISC %02X%02X,Y",   3, 7, nullptr      , 0xFB}, // (undoc) INC + SBC abs,Y
    {AddrMode::AbsoluteX,   "NOP %02X%02X,X",   3, 4, nullptr      , 0xFC}, // (undoc) NOP abs,X
    {AddrMode::AbsoluteX,   "SBC %02X%02X,X",   3, 4, &MOS6502::SBC, 0xFD}, // SBC abs,X
    {AddrMode::AbsoluteX,   "INC %02X%02X,X",   3, 7, &MOS6502::INC, 0xFE}, // INC abs,X
    {AddrMode::AbsoluteX,   "ISC %02X%02X,X",   3, 7, nullptr      , 0xFF}  // (undoc) INC + SBC abs,X
    };
    void ADC(AddrMode);
    void AND(AddrMode);
    void ASL(AddrMode);
    void BCC(AddrMode);
    void BCS(AddrMode);
    void BEQ(AddrMode);
    void BIT(AddrMode);
    void BMI(AddrMode);
    void BNE(AddrMode);
    void BPL(AddrMode);
    void BRK(AddrMode);
    void BVC(AddrMode);
    void BVS(AddrMode);
    void CLC(AddrMode);
    void CLD(AddrMode);
    void CLI(AddrMode);
    void CLV(AddrMode);
    void CMP(AddrMode);
    void CPX(AddrMode);
    void CPY(AddrMode);
    void DEC(AddrMode);
    void DEX(AddrMode);
    void DEY(AddrMode);
    void EOR(AddrMode);
    void INC(AddrMode);
    void INX(AddrMode);
    void INY(AddrMode);
    void JMP(AddrMode);
    void JSR(AddrMode);
    void LDA(AddrMode);
    void LDX(AddrMode);
    void LDY(AddrMode);
    void LSR(AddrMode);
    void NOP(AddrMode);
    void ORA(AddrMode);
    void PHA(AddrMode);
    void PHP(AddrMode);
    void PLA(AddrMode);
    void PLP(AddrMode);
    void ROL(AddrMode);
    void ROR(AddrMode);
    void RTI(AddrMode);
    void RTS(AddrMode);
    void SBC(AddrMode);
    void SEC(AddrMode);
    void SED(AddrMode);
    void SEI(AddrMode);
    void STA(AddrMode);
    void STX(AddrMode);
    void STY(AddrMode);
    void TAX(AddrMode);
    void TAY(AddrMode);
    void TSX(AddrMode);
    void TXA(AddrMode);
    void TXS(AddrMode);
    void TYA(AddrMode);
    
    void getParams(OPERAND &op);

    uint16_t getAddress(AddrMode m);
    uint8_t  getValue  (AddrMode m);

    inline void setNZ(uint8_t);

  public:
    OPERAND fetch();
    int cycle();

};






