//===- Z80RegisterInfo.td - Describe the Z80 Register Info ----*- tblgen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Declaration that describes the Z80 register file
//===----------------------------------------------------------------------===//

class Z80Reg<string n>
  : Register<n> {
  let Namespace = "Z80";
}

class Z80SubRegIndex : SubRegIndex {
  let Namespace = "Z80";
}

def subreg_hi : Z80SubRegIndex;
def subreg_lo : Z80SubRegIndex;

class Z80RegWithSubRegs<string n, list<Register> subregs>
  : RegisterWithSubRegs<n, subregs> {
  let SubRegIndices = [subreg_hi, subreg_lo];
  let CoveredBySubRegs = 1;
  let Namespace = "Z80";
}

class Z80Reg8Class<dag reglist>
  : RegisterClass<"Z80", [i8], 8, reglist> {
}

class Z80Reg16Class<dag reglist>
  : RegisterClass<"Z80", [i16], 8, reglist> {
}

//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//

// 8 bit registers
def A : Z80Reg<"a">;
def B : Z80Reg<"b">;
def C : Z80Reg<"c">;
def D : Z80Reg<"d">;
def E : Z80Reg<"e">;
def H : Z80Reg<"h">;
def L : Z80Reg<"l">;

// Flags register
def FLAGS : Z80Reg<"f">;

// 16 bit registers
def BC : Z80RegWithSubRegs<"bc", [B, C]>;
def DE : Z80RegWithSubRegs<"de", [D, E]>;
def HL : Z80RegWithSubRegs<"hl", [H, L]>;
def AF : Z80RegWithSubRegs<"af", [A, FLAGS]>;

def SP : Z80Reg<"sp">;
def PC : Z80Reg<"pc">;

def GR8  : Z80Reg8Class<(add A, B, C, D, E, H, L)>;
def GR16 : Z80Reg16Class<(add BC, DE, HL)>;
