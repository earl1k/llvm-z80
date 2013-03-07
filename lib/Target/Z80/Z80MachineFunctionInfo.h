//===-- Z80MachineFuctionInfo.h - Z80 machine function info -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares Z80-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef Z80MACHINEFUNCTIONINFO_H
#define Z80MACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {
  class Z80MachineFunctionInfo : public MachineFunctionInfo {
    virtual void anchor();

    // needFP - Flag which indicate to use FP
    bool needFP;

    // CalleeSavedFrameSize - Size of the callee-saved register portion of the
    // stack frame in bytes.
    unsigned CalleeSavedFrameSize;
  public:
    explicit Z80MachineFunctionInfo(MachineFunction &MF)
      : needFP(false), CalleeSavedFrameSize(0) {}

    unsigned getCalleeSavedFrameSize() { return CalleeSavedFrameSize; }
    void setCalleeSavedFrameSize(unsigned bytes) {
      CalleeSavedFrameSize = bytes;
    }
    bool isNeedFP() const { return needFP; }
    void setNeedFP() { needFP = true; }
  }; // end class Z80MachineFunctionInfo
} // end namespace llvm

#endif
