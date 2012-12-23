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
// Instruction list.
//===----------------------------------------------------------------------===//

def NOP : Instruction {
  let Namespace = "Z80";

  dag OutOperandList = (outs);
  dag InOperandList = (ins);
}