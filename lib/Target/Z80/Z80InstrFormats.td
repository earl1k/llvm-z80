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

  dag OutOperandList = outs;
  dag InOperandList = ins;

  let AsmString = asmstr;
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
  bits<8> imm;
  let Inst{15-8} = imm;
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
  bits<16> imm;
  let Inst{23-8} = imm;
}
