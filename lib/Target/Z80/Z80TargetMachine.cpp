//===-- Z80TargetMachine.cpp - Define TargetMachine for the Z80 -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the Z80 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#include "Z80TargetMachine.h"
#include "Z80.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeZ80Target() {
  RegisterTargetMachine<Z80TargetMachine> X(TheZ80Target);
}

Z80TargetMachine::Z80TargetMachine(const Target &T, StringRef TT, StringRef CPU,
  StringRef FS, const TargetOptions &Options, Reloc::Model RM,
  CodeModel::Model CM, CodeGenOpt::Level OL)
  : LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
  FrameLowering(*this),
  DL("e-p:16:8:8-i8:8:8-i16:8:8-n8:16"),
  InstrInfo(*this), TSInfo(*this), TLInfo(*this)
{}

namespace {
  class Z80PassConfig : public TargetPassConfig {
  public:
    Z80PassConfig(Z80TargetMachine *TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}
    Z80TargetMachine &getZ80TargetMachine() const {
      return getTM<Z80TargetMachine>();
    }
    virtual bool addInstSelector();
  }; // end class Z80PassConfig
} // end namespace

TargetPassConfig *Z80TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new Z80PassConfig(this, PM);
}

bool Z80PassConfig::addInstSelector() {
  addPass(createZ80ISelDAG(getZ80TargetMachine(), getOptLevel()));
  return false;
}
