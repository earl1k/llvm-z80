//===-- Z80TargetFrameLowering.h - Define frame lowering for Z80 -*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class implements Z80-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef Z80FRAMELOWERING_H
#define Z80FRAMELOWERING_H

#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
  class Z80TargetMachine;

  class Z80FrameLowering : public TargetFrameLowering {
    const Z80TargetMachine &TM;
  public:
    explicit Z80FrameLowering(const Z80TargetMachine &tm);

    void emitPrologue(MachineFunction &MF) const;
    void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;

    bool hasFP(const MachineFunction &MF) const;
  }; // end class Z80FrameLowering
} // end namespace llvm

#endif
