//===-- Z80CallingConv.td - Z80 Calling Conventions --------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the Z80 architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Z80 Return Value Calling Convention
//===----------------------------------------------------------------------===//

def RetCC_Z80 : CallingConv<[
  // i8 are returned in register A
  CCIfType<[i8], CCAssignToReg<[A]>>,

  // i16 are returned in register HL
  CCIfType<[i16], CCAssignToReg<[HL]>>
]>;

//===----------------------------------------------------------------------===//
// Z80 Argument Calling Conventions
//===----------------------------------------------------------------------===//

def CC_Z80 : CallingConv<[
  // Assign i8 arguments in registers
  CCIfType<[i8], CCAssignToReg<[A, B, C]>>,

  // Assign i16 arguments in registers
  CCIfType<[i16], CCAssignToReg<[HL, BC, DE]>>
]>;

//===----------------------------------------------------------------------===//
// Callee-saved Registers.
//===----------------------------------------------------------------------===//

def CSR_NoRegs : CalleeSavedRegs<(add)>;

def CSR_DE : CalleeSavedRegs<(add DE)>;
