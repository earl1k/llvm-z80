//===-- Z80InstrInfo.cpp - Z80 Instruction Information --------------------===//
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

#include "Z80InstrInfo.h"
#include "Z80.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#define GET_INSTRINFO_CTOR
#include "Z80GenInstrInfo.inc"

using namespace llvm;

Z80InstrInfo::Z80InstrInfo(Z80TargetMachine &tm)
  : RI(tm, *this), TM(tm)
{}

void Z80InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator I, DebugLoc DL,
  unsigned DestReg, unsigned SrcReg, bool KillSrc) const
{
  if (Z80::GR8RegClass.contains(DestReg, SrcReg))
  {
    BuildMI(MBB, I, DL, get(Z80::LD8rr), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
  }
  else llvm_unreachable("Imposible reg-to-reg copy");
}
