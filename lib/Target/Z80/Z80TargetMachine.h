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
#include "Z80FrameLowering.h"
#include "Z80ISelLowering.h"
#include "Z80InstrInfo.h"
#include "Z80SelectionDAGInfo.h"
#include "llvm/DataLayout.h"

namespace llvm {
  class Z80TargetMachine : public LLVMTargetMachine {
    const DataLayout DL;  // Calculates type size & alignment
    Z80FrameLowering FrameLowering;
    Z80InstrInfo InstrInfo;
    Z80SelectionDAGInfo TSInfo;
    Z80TargetLowering TLInfo;
  public:
    Z80TargetMachine(const Target &T, StringRef TT, StringRef CPU,
      StringRef FS, const TargetOptions &Options, Reloc::Model RM,
      CodeModel::Model CM, CodeGenOpt::Level OL);
    virtual const DataLayout *getDataLayout() const { return &DL; }
    virtual const Z80FrameLowering *getFrameLowering() const {
      return &FrameLowering;
    }
    virtual const Z80InstrInfo *getInstrInfo() const { return &InstrInfo; }
    virtual const Z80RegisterInfo *getRegisterInfo() const {
      return &getInstrInfo()->getRegisterInfo();
    }
    virtual const Z80SelectionDAGInfo *getSelectionDAGInfo() const {
      return &TSInfo;
    }
    virtual const Z80TargetLowering *getTargetLowering() const {
      return &TLInfo;
    }
  }; // end class Z80TargetMachine
} // end namespace llvm

#endif
