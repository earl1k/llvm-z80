//===-- Z80InstrFormats.td - Z80 Instruction Formats -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Z80 Instruction Format Definitions.
//===----------------------------------------------------------------------===//

class Z80Inst<dag outs, dag ins, string asmstr>
  : Instruction {
  let Namespace = "Z80";

  field bits<32> Inst;

  dag OutOperandList = outs;
  dag InOperandList = ins;

  let AsmString = asmstr;
}

class I<bits<8> opc, dag outs, dag ins, string asmstr, list<dag> pattern>
  : Z80Inst<outs, ins, asmstr> {
  let Inst{7-0} = opc;
  let Size = 1;
  let Pattern = pattern;
}
