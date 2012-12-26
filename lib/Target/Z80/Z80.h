//===- Z80.h - Define TargetMachine for Z80 -------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// Z80 back-end.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_Z80_H
#define TARGET_Z80_H

#include "MCTargetDesc/Z80MCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class Z80TargetMachine;
  class FunctionPass;

  FunctionPass *createZ80ISelDAG(Z80TargetMachine &TM, CodeGenOpt::Level OptLevel);
} // end namespace llvm

#endif
