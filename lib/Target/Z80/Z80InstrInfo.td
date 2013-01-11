//===-- Z80InstrInfo.td - Z80 Instruction defs -------------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the Z80 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Z80 Instruction Format Definitions.
//===----------------------------------------------------------------------===//

include "Z80InstrFormats.td"

//===----------------------------------------------------------------------===//
// Z80 Specific Node Definitions.
//===----------------------------------------------------------------------===//
def Z80ret : SDNode<"Z80ISD::RET", SDTNone, [SDNPHasChain, SDNPOptInGlue]>;

//===----------------------------------------------------------------------===//
// Instruction list.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Control Flow Instructions.
//===----------------------------------------------------------------------===//
let isReturn = 1, isTerminator = 1, isBarrier = 1 in
def RET : I<0xC9, (outs), (ins), "ret", [(Z80ret)]>;

//===----------------------------------------------------------------------===//
// Load Instructions.
//===----------------------------------------------------------------------===//
let neverHasSideEffects = 1 in
def LD8rr : IRyRz<0x40, (outs GR8:$dst), (ins GR8:$src),
  "ld\t{$dst, $src}", []>;

let isReMaterializable = 1, isAsCheapAsAMove = 1 in {
  def LD8ri  : IRyI8<0x06,  (outs GR8:$dst),  (ins i8imm:$src),
    "ld\t{$dst, $src}", [(set GR8:$dst, imm:$src)]>;
  def LD16ri : IRpI16<0x01, (outs GR16:$dst), (ins i16imm:$src),
    "ld\t{$dst, $src}", [(set GR16:$dst, imm:$src)]>;
}

//===----------------------------------------------------------------------===//
// Arithmetic Instructions
//===----------------------------------------------------------------------===//

class ALUOpType<bits<3> val> {
  field bits<3> Value = val;
}

def ALU_ADD : ALUOpType<0>;
def ALU_ADC : ALUOpType<1>;
def ALU_SUB : ALUOpType<2>;
def ALU_SBC : ALUOpType<3>;
def ALU_AND : ALUOpType<4>;
def ALU_XOR : ALUOpType<5>;
def ALU_OR  : ALUOpType<6>;
def ALU_CP  : ALUOpType<7>;

class ALUIr<ALUOpType alu, string asmstr, SDNode ops>
  : IRz<0x80, (outs), (ins GR8:$src), asmstr, [(set A, (ops A, GR8:$src))]> {
  ALUOpType aluType = alu;
  let Inst{5-3} = aluType.Value;
}
class ALUIi<ALUOpType alu, string asmstr, SDNode ops>
  : II8<0xC6, (outs), (ins i8imm:$src), asmstr, [(set A, (ops A, imm:$src))]> {
  ALUOpType aluType = alu;
  let Inst{5-3} = aluType.Value;
}

multiclass ALUI<ALUOpType alu, string asmstr, SDNode ops> {
  def r : ALUIr<alu, asmstr, ops>;
  def i : ALUIi<alu, asmstr, ops>;
}

let Uses = [A], Defs = [A, FLAGS] in {
  let isCommutable = 1 in {
    defm ADD8 : ALUI<ALU_ADD, "add\t{a, $src}", add>;
    defm ADC8 : ALUI<ALU_ADC, "adc\t{a, $src}", adde>;
    defm AND8 : ALUI<ALU_AND, "and\t{$src}", and>;
    defm XOR8 : ALUI<ALU_XOR, "xor\t{$src}", xor>;
    defm OR8  : ALUI<ALU_OR,  "or\t{$src}",  or>;
  }
  defm SUB8 : ALUI<ALU_SUB, "sub\t{$src}", sub>;
  defm SBC8 : ALUI<ALU_SBC, "sbc\t{a, $src}", sube>;
}

let Uses = [HL], Defs = [HL, FLAGS] in {
  let isCommutable = 1 in {
    def ADD16r : IRp<0x09, (outs), (ins GR16:$src),
      "add\t{hl, $src}", [(set HL, (add HL, GR16:$src))]>;
    def ADC16r : IRp<0x4A, (outs), (ins GR16:$src),
      "adc\t{hl, $src}", [(set HL, (adde HL, GR16:$src))]>, ED;
  }
  def SBC16r : IRp<0x42, (outs), (ins GR16:$src),
    "sbc\t{hl, $src}", [(set HL, (sube HL, GR16:$src))]>, ED;
}

def : Pat<(addc A, GR8:$src), (ADD8r GR8:$src)>;
def : Pat<(addc A, imm:$src), (ADD8r imm:$src)>;

def : Pat<(addc HL, GR16:$src), (ADD16r GR16:$src)>;

def : Pat<(subc A, GR8:$src), (SUB8r GR8:$src)>;
def : Pat<(subc A, imm:$src), (SUB8r imm:$src)>;

//===----------------------------------------------------------------------===//
// Non-Instruction Patterns.
//===----------------------------------------------------------------------===//

// anyext
def : Pat<(i16 (anyext GR8:$src)),
  (SUBREG_TO_REG (i16 0), GR8:$src, subreg_lo)>;

// trunc
def : Pat<(i8 (trunc GR16:$src)), (EXTRACT_SUBREG GR16:$src, subreg_lo)>;
