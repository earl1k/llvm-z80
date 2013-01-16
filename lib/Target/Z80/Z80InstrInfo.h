//===-- Z80InstrInfo.h - Z80 Instruction Information ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Z80 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef Z80INSTRINFO_H
#define Z80INSTRINFO_H

#include "Z80.h"
#include "Z80RegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "Z80GenInstrInfo.inc"

namespace llvm {
  namespace Z80 {
    enum CondCode {
      COND_NZ = 0,
      COND_Z  = 1,
      COND_NC = 2,
      COND_C  = 3,
      COND_PO = 4,
      COND_PE = 5,
      COND_P  = 6,
      COND_M  = 7,

      COND_INVALID
    };
  } // end namespace Z80
  class Z80InstrInfo : public Z80GenInstrInfo {
    const Z80RegisterInfo RI;
    Z80TargetMachine &TM;
  public:
    explicit Z80InstrInfo(Z80TargetMachine &tm);

    virtual const Z80RegisterInfo &getRegisterInfo() const { return RI; }

    virtual void copyPhysReg(MachineBasicBlock &MBB,
      MachineBasicBlock::iterator I, DebugLoc DL,
      unsigned DestReg, unsigned SrcReg, bool KillSrc) const;

    virtual MachineInstr* commuteInstruction(MachineInstr *MI,
      bool NewMI = false) const;
  }; // end class Z80InstrInfo
} // end namespace llvm

#endif
