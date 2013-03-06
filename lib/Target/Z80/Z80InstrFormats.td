//===-- Z80InstrFormats.td - Z80 Instruction Formats -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Naming of opcode bits encoding is borrowed from this page:
// http://z80.info/decoding.htm
// 7 6 5 4 3 2 1 0
// x x y y y z z z
//     p p q
//
//===-------------------- --------------------------------------------------===//

class Prefix<bits<3> n> {
  field bits<3> Prefix = n;
}

class DD       : Prefix<1>;
class FD       : Prefix<2>;
class ED       : Prefix<3>;
class CB       : Prefix<4>;
class DDCB     : Prefix<5>;
class FDCB     : Prefix<6>;

//===----------------------------------------------------------------------===//
// Z80 Instruction Format Definitions.
//===----------------------------------------------------------------------===//
//
// I - Instruction with no operands.
// I<xx> - Instruction with operands.
//
// Operands:
//  R<x> - Register from record x (example Rz, Ry, Rp)
//  I<n> - Immediate with n-bits (example I8, I16)
//
// Examples:
// II8   - Instruction with i8 immediate data.
// IRyRz - Instruction with Register-Register operands, which encoding from
//         y and z record.
// IRyI8 - Instruction with Register from y record and i8 immediate data.
//
//===----------------------------------------------------------------------===//

class Z80Inst<dag outs, dag ins, string asmstr>
  : Instruction {
  let Namespace = "Z80";

  field bits<32> Inst;
  bits<3> Prefix = 0;

  dag OutOperandList = outs;
  dag InOperandList = ins;

  let TSFlags{2-0} = Prefix;

  let AsmString = asmstr;
}

class PseudoI<dag outs, dag ins, list<dag> pattern>
  : Z80Inst<outs, ins, ""> {
  let isPseudo = 1;
  let Pattern = pattern;
}

//===----------------------------------------------------------------------===//
// Simple Instruction
//===----------------------------------------------------------------------===//

class I<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern,
  int size = 1> : Z80Inst<outs, ins, asmstr> {
  let Inst{7-0} = opc;
  let Size = size;
  let Pattern = pattern;
}

//===----------------------------------------------------------------------===//
// Simple Instruction with R[y]
//===----------------------------------------------------------------------===//

class IRy<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern,
  int size = 1> : I<opc, outs, ins, asmstr, pattern, size> {
  bits<3> Ry;
  let Inst{5-3} = Ry;
}

//===----------------------------------------------------------------------===//
// Simple Instruction with R[y], I[8]
//===----------------------------------------------------------------------===//

class IRyI8<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern>
  : IRy<opc, outs, ins, asmstr, pattern, 2> {
  bits<8> Imm;
  let Inst{15-8} = Imm;
}

//===----------------------------------------------------------------------===//
// Simple Instruction with R[y], I[16]
//===----------------------------------------------------------------------===//

class IRyI16<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern>
  : IRy<opc, outs, ins, asmstr, pattern, 3> {
  bits<16> Imm;
  let Inst{23-8} = Imm;
}

//===----------------------------------------------------------------------===//
// Simple Instruction with R[y], R[z]
//===----------------------------------------------------------------------===//

class IRyRz<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern>
  : IRy<opc, outs, ins, asmstr, pattern> {
  bits<3> Rz;
  let Inst{2-0} = Rz;
}

//===----------------------------------------------------------------------===//
// Simple Instruction with R[z]
//===----------------------------------------------------------------------===//

class IRz<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern,
  int size = 1> : I<opc, outs, ins, asmstr, pattern, size> {
  bits<3> Rz;
  let Inst{2-0} = Rz;
}

//===----------------------------------------------------------------------===//
// Simple Instruction with R[z], I[8]
//===----------------------------------------------------------------------===//

class IRzI8<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern>
  : IRz<opc, outs, ins, asmstr, pattern, 2> {
  bits<8> Imm;
  let Inst{15-8} = Imm;
}

//===----------------------------------------------------------------------===//
// Simple Instruction with R[p]
//===----------------------------------------------------------------------===//

class IRp<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern,
  int size = 1> : I<opc, outs, ins, asmstr, pattern, size> {
  bits<2> Rp;
  let Inst{5-4} = Rp;
}

//===----------------------------------------------------------------------===//
// Simple Instruction with R[p], I[16]
//===----------------------------------------------------------------------===//

class IRpI16<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern>
  : IRp<opc, outs, ins, asmstr, pattern, 3> {
  bits<16> Imm;
  let Inst{23-8} = Imm;
}

//===----------------------------------------------------------------------===//
// Simple Instruction with I[8]
//===----------------------------------------------------------------------===//

class II8<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern,
  int size = 2> : I<opc, outs, ins, asmstr, pattern, size> {
  bits<8> Imm;
  let Inst{15-8} = Imm;
}

//===----------------------------------------------------------------------===//
// Simple Instruction with I[8], R[z]
//===----------------------------------------------------------------------===//

class II8Rz<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern>
  : II8<opc, outs, ins, asmstr, pattern> {
  bits<3> Rz;
  let Inst{2-0} = Rz;
}

//===----------------------------------------------------------------------===//
// Simple Instruction with I[8], I[8]
//===----------------------------------------------------------------------===//

class II8I8<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern>
  : II8<opc, outs, ins, asmstr, pattern, 3> {
  bits<8> Imm2;
  let Inst{23-16} = Imm2;
}

//===----------------------------------------------------------------------===//
// Simple Instruction with I[16]
//===----------------------------------------------------------------------===//

class II16<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern>
  : I<opc, outs, ins, asmstr, pattern, 3> {
  bits<16> Imm;
  let Inst{23-8} = Imm;
}

//===----------------------------------------------------------------------===//
// Simple Instruction RST
//===----------------------------------------------------------------------===//

class Irst<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern>
  : I<opc, outs, ins, asmstr, pattern> {
  bits<16> Imm;
  let Inst{5-3} = Imm{5-3};
}
