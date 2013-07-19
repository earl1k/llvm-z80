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

class Z80Reg<string n, bits<16> Enc = 0, int Cost = 0>
  : Register<n> {
  let Namespace  = "Z80";
  let HWEncoding = Enc;
  let CostPerUse = Cost;
}

class Z80SubRegIndex<int size, int offset = 0> : SubRegIndex<size, offset> {
  let Namespace = "Z80";
}

def subreg_hi : Z80SubRegIndex<8, 8>;
def subreg_lo : Z80SubRegIndex<8, 0>;

class Z80RegWithSubRegs<string n, bits<16> Enc = 0, list<Register> subregs = [], int Cost = 0>
  : Z80Reg<n, Enc, Cost> {
  let SubRegs = subregs;
  let SubRegIndices = [subreg_hi, subreg_lo];
  let CoveredBySubRegs = 1;
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
def A : Z80Reg<"a", 7>;
def B : Z80Reg<"b", 0>;
def C : Z80Reg<"c", 1>;
def D : Z80Reg<"d", 2>;
def E : Z80Reg<"e", 3>;
def H : Z80Reg<"h", 4>;
def L : Z80Reg<"l", 5>;
def XH : Z80Reg<"xh", 4, 1>;
def XL : Z80Reg<"xl", 5, 1>;
def YH : Z80Reg<"yh", 4, 1>;
def YL : Z80Reg<"yl", 5, 1>;

// Flags register
def FLAGS : Z80Reg<"f">;

// 16 bit registers
def BC : Z80RegWithSubRegs<"bc", 0, [B, C]>;
def DE : Z80RegWithSubRegs<"de", 1, [D, E]>;
def HL : Z80RegWithSubRegs<"hl", 2, [H, L]>;
def AF : Z80RegWithSubRegs<"af", 3, [A, FLAGS]>;
def IX : Z80RegWithSubRegs<"ix", 2, [XH, XL], 1>;
def IY : Z80RegWithSubRegs<"iy", 2, [YH, YL], 1>;

def SP : Z80Reg<"sp">;
def PC : Z80Reg<"pc">;

def GR8  : Z80Reg8Class<(add A, B, C, D, E, H, L)>;
def GR16 : Z80Reg16Class<(add BC, DE, HL, IX, IY)>;
def BR16 : Z80Reg16Class<(add BC, DE, HL)>;
def IR16 : Z80Reg16Class<(add IX, IY)>;

def EXR16 : Z80Reg16Class<(add HL, DE)>;
