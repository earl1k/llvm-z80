//===-- Z80RegisterInfo.h - Z80 Register Information Impl -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Z80 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef Z80REGISTERINFO_H
#define Z80REGISTERINFO_H

#define GET_REGINFO_HEADER
#include "Z80GenRegisterInfo.inc"

namespace llvm {
  class TargetInstrInfo;
  class Z80TargetMachine;

  class Z80RegisterInfo : public Z80GenRegisterInfo {
    Z80TargetMachine &TM;
    const TargetInstrInfo &TII;
  public:
    Z80RegisterInfo(Z80TargetMachine &tm, const TargetInstrInfo &tii);

    // Code Generation virtual methods...
    const uint16_t *getCalleeSavedRegs(const MachineFunction *MF = 0) const;

    BitVector getReservedRegs(const MachineFunction &MF) const;

    void eliminateCallFramePseudoInstr(MachineFunction &MF,
      MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const;

    void eliminateFrameIndex(MachineBasicBlock::iterator I,
      int SPAdj, RegScavenger *RS = NULL) const;

    // Debug information queries
    unsigned getFrameRegister(const MachineFunction &MF) const;
  }; // end class Z80RegisterInfo
} // end namespace llvm

#endif
