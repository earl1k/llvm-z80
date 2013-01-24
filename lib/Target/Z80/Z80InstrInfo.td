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
// Z80 Type Profiles.
//===----------------------------------------------------------------------===//
def SDT_Z80Call         : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;
def SDT_Z80CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>]>;
def SDT_Z80CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;
def SDT_Z80Cp           : SDTypeProfile<0, 2, [SDTCisSameAs<0, 1>]>;
def SDT_Z80SelectCC     : SDTypeProfile<1, 3, [SDTCisSameAs<0, 1>,
                                               SDTCisSameAs<1, 2>,
                                               SDTCisVT<3, i8>]>;
def SDT_Z80BrCC         : SDTypeProfile<0, 2, [SDTCisVT<0, i8>,
                                           SDTCisVT<1, OtherVT>]>;
//===----------------------------------------------------------------------===//
// Z80 Specific Node Definitions.
//===----------------------------------------------------------------------===//
def Z80call          : SDNode<"Z80ISD::CALL", SDT_Z80Call,
                     [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;
def Z80callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_Z80CallSeqStart,
                     [SDNPHasChain, SDNPOutGlue]>;
def Z80callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_Z80CallSeqEnd,
                     [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def Z80ret           : SDNode<"Z80ISD::RET", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue]>;
def Z80rlc           : SDNode<"Z80ISD::RLC", SDTIntUnaryOp, [SDNPOutGlue]>;
def Z80rrc           : SDNode<"Z80ISD::RRC", SDTIntUnaryOp, [SDNPOutGlue]>;
def Z80rl            : SDNode<"Z80ISD::RL",  SDTIntUnaryOp,
                     [SDNPOutGlue, SDNPInGlue]>;
def Z80rr            : SDNode<"Z80ISD::RR",  SDTIntUnaryOp, 
                     [SDNPOutGlue, SDNPInGlue]>;
def Z80sla           : SDNode<"Z80ISD::SLA", SDTIntUnaryOp, [SDNPOutGlue]>;
def Z80sra           : SDNode<"Z80ISD::SRA", SDTIntUnaryOp, [SDNPOutGlue]>;
def Z80sll           : SDNode<"Z80ISD::SLL", SDTIntUnaryOp, [SDNPOutGlue]>;
def Z80srl           : SDNode<"Z80ISD::SRL", SDTIntUnaryOp, [SDNPOutGlue]>;
def Z80cp            : SDNode<"Z80ISD::CP", SDT_Z80Cp, [SDNPOutGlue]>;
def Z80selectcc      : SDNode<"Z80ISD::SELECT_CC", SDT_Z80SelectCC,
                     [SDNPInGlue]>;
def Z80brcc          : SDNode<"Z80ISD::BR_CC", SDT_Z80BrCC,
                     [SDNPHasChain, SDNPInGlue]>;
def Z80scf           : SDNode<"Z80ISD::SCF", SDTNone, [SDNPOutGlue]>;
def Z80ccf           : SDNode<"Z80ISD::CCF", SDTNone,
                     [SDNPOutGlue, SDNPInGlue]>;
//===----------------------------------------------------------------------===//
// Operand Definitions.
//===----------------------------------------------------------------------===//

def brtarget : Operand<OtherVT> {
  let EncoderMethod = "getBREncoding";
}

def calltarget : Operand<iPTR> {
  let EncoderMethod = "getBREncoding";
}

def cc : Operand<i8> {
  let PrintMethod = "printCCOperand";
}

//===----------------------------------------------------------------------===//
// Instruction list.
//===----------------------------------------------------------------------===//
let Defs = [FLAGS] in {
  def SCF : I<0x37, (outs), (ins), "scf", [(Z80scf)]>;
  let Uses = [FLAGS] in
  def CCF : I<0x3F, (outs), (ins), "ccf", [(Z80ccf)]>;
}

let Defs = [SP], Uses = [SP] in {
  def ADJCALLSTACKDOWN : PseudoI<(outs), (ins i16imm:$amt),
    [(Z80callseq_start timm:$amt)]>;
  def ADJCALLSTACKUP   : PseudoI<(outs), (ins i16imm:$amt1, i16imm:$amt2),
    [(Z80callseq_end timm:$amt1, timm:$amt2)]>;
}

let usesCustomInserter = 1 in {
  def SELECT8  : PseudoI<(outs GR8:$dst),  (ins GR8:$src,  GR8:$src2,  cc:$cc),
    [(set GR8:$dst,  (Z80selectcc GR8:$src,  GR8:$src2,  imm:$cc))]>;
  def SELECT16 : PseudoI<(outs GR16:$dst), (ins GR16:$src, GR16:$src2, cc:$cc),
    [(set GR16:$dst, (Z80selectcc GR16:$src, GR16:$src2, imm:$cc))]>;
}
//===----------------------------------------------------------------------===//
// Control Flow Instructions.
//===----------------------------------------------------------------------===//
let isCall = 1, Uses = [SP] in {
  def CALL : II16<0xCD, (outs), (ins calltarget:$dst, variable_ops),
    "call\t{$dst}", [(Z80call imm:$dst)]>;
}

def : Pat<(Z80call (i16 tglobaladdr:$dst)), (CALL tglobaladdr:$dst)>;
def : Pat<(Z80call (i16 texternalsym:$dst)), (CALL texternalsym:$dst)>;

let isReturn = 1, isTerminator = 1, isBarrier = 1 in
def RET : I<0xC9, (outs), (ins), "ret", [(Z80ret)]>;

let isBranch = 1, isTerminator = 1 in {
  let isBarrier = 1 in
  def JP : II16<0xC3, (outs), (ins brtarget:$dst),
    "jp\t{$dst}", [(br bb:$dst)]>;
  let Uses = [FLAGS] in
  def JPCC : IRyI16<0xC2, (outs), (ins cc:$cc, brtarget:$dst),
    "jp\t{$cc, $dst}", [(Z80brcc imm:$cc, bb:$dst)]>;
}

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

class ALUOpType<bits<3> val, SDNode ops> {
  field bits<3> Value = val;
  SDNode Node = ops;
}

def ALU_ADD : ALUOpType<0, add>;
def ALU_ADC : ALUOpType<1, adde>;
def ALU_SUB : ALUOpType<2, sub>;
def ALU_SBC : ALUOpType<3, sube>;
def ALU_AND : ALUOpType<4, and>;
def ALU_XOR : ALUOpType<5, xor>;
def ALU_OR  : ALUOpType<6, or>;
def ALU_CP  : ALUOpType<7, Z80cp>;

class ALUIr<ALUOpType alu, string asmstr, list<dag> pattern>
  : IRz<0x80, (outs), (ins GR8:$src), asmstr, pattern> {
  ALUOpType aluType = alu;
  let Inst{5-3} = aluType.Value;
}
class ALUIi<ALUOpType alu, string asmstr, list<dag> pattern>
  : II8<0xC6, (outs), (ins i8imm:$src), asmstr, pattern> {
  ALUOpType aluType = alu;
  let Inst{5-3} = aluType.Value;
}

multiclass ALUI<ALUOpType alu, string asmstr> {
  def r : ALUIr<alu, asmstr, [(set A, (alu.Node A, GR8:$src))]>;
  def i : ALUIi<alu, asmstr, [(set A, (alu.Node A, imm:$src))]>;
}

multiclass ALUICP<ALUOpType alu, string asmstr> {
  def r : ALUIr<alu, asmstr, [(Z80cp A, GR8:$src)]>;
  def i : ALUIi<alu, asmstr, [(Z80cp A, imm:$src)]>;
}

class RSOpType<bits<3> val, SDNode ops> {
  field bits<3> Value = val;
  SDNode Node = ops;
}

def RS_RLC : RSOpType<0, Z80rlc>;
def RS_RRC : RSOpType<1, Z80rrc>;
def RS_RL  : RSOpType<2, Z80rl>;
def RS_RR  : RSOpType<3, Z80rr>;
def RS_SLA : RSOpType<4, Z80sla>;
def RS_SRA : RSOpType<5, Z80sra>;
def RS_SLL : RSOpType<6, Z80sll>;
def RS_SRL : RSOpType<7, Z80srl>;

class RSIr<RSOpType rs, string asmstr>
  : IRz<0x00, (outs GR8:$dst), (ins GR8:$src), asmstr,
  [(set GR8:$dst, (rs.Node GR8:$src))]>, CB {
  let Inst{5-3} = rs.Value;
  let Constraints = "$src = $dst";
}

multiclass RSI<RSOpType rs, string asmstr> {
  def r : RSIr<rs, asmstr>;
}

let Uses = [A], Defs = [A, FLAGS] in {
  let isCommutable = 1 in {
    defm ADD8 : ALUI<ALU_ADD, "add\t{a, $src}">;
    let Uses = [A, FLAGS] in
    defm ADC8 : ALUI<ALU_ADC, "adc\t{a, $src}">;
    defm AND8 : ALUI<ALU_AND, "and\t{$src}">;
    defm XOR8 : ALUI<ALU_XOR, "xor\t{$src}">;
    defm OR8  : ALUI<ALU_OR,  "or\t{$src}">;
  }
  defm SUB8 : ALUI<ALU_SUB, "sub\t{$src}">;
  let Uses = [A, FLAGS] in
  defm SBC8 : ALUI<ALU_SBC, "sbc\t{a, $src}">;
  let Defs = [FLAGS] in
  defm CP8  : ALUICP<ALU_CP, "cp\t{a, $src}">;
}

let Defs = [FLAGS] in {
  defm RLC8 : RSI<RS_RLC, "rlc\t{$src}">;
  defm RRC8 : RSI<RS_RRC, "rrc\t{$src}">;
  let Uses = [FLAGS] in {
    defm RL8  : RSI<RS_RL,  "rl\t{$src}">;
    defm RR8  : RSI<RS_RR,  "rr\t{$src}">;
  }
  defm SLA8 : RSI<RS_SLA, "sla\t{$src}">;
  defm SRA8 : RSI<RS_SRA, "sra\t{$src}">;
  defm SLL8 : RSI<RS_SLL, "sll\t{$src}">;
  defm SRL8 : RSI<RS_SRL, "srl\t{$src}">;
}

let Uses = [HL], Defs = [HL, FLAGS] in {
  let isCommutable = 1 in {
    def ADD16r : IRp<0x09, (outs), (ins GR16:$src),
      "add\t{hl, $src}", [(set HL, (add HL, GR16:$src))]>;
    let Uses = [HL, FLAGS] in
    def ADC16r : IRp<0x4A, (outs), (ins GR16:$src),
      "adc\t{hl, $src}", [(set HL, (adde HL, GR16:$src))]>, ED;
  }
  let Uses = [HL, FLAGS] in
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
