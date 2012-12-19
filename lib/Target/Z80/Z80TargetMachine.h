//==-- Z80TargetMachine.h - Define TargetMachine for Z80 ---------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Z80 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef Z80TARGETMACHINE_H
#define Z80TARGETMACHINE_H

#include "Z80.h"

namespace llvm {
  class Z80TargetMachine : public LLVMTargetMachine {
  public:
    Z80TargetMachine(const Target &T, StringRef TT, StringRef CPU,
      StringRef FS, const TargetOptions &Options, Reloc::Model RM,
      CodeModel::Model CM, CodeGenOpt::Level OL);
  }; // end class Z80TargetMachine
} // end namespace llvm

#endif
