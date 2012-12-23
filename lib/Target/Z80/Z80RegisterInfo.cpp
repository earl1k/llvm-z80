//===-- Z80RegisterInfo.cpp - Z80 Register Information --------------------===//
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

#include "Z80RegisterInfo.h"
#include "Z80.h"
#include "llvm/ADT/BitVector.h"

#define GET_REGINFO_TARGET_DESC
#include "Z80GenRegisterInfo.inc"

using namespace llvm;

Z80RegisterInfo::Z80RegisterInfo(Z80TargetMachine &tm, const TargetInstrInfo &tii)
  : Z80GenRegisterInfo(Z80::PC), TM(tm), TII(tii)
{}

const uint16_t* Z80RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const
{
  static const uint16_t CalleeSavedRegs[] = {
    0
  };
  return CalleeSavedRegs;
}

BitVector Z80RegisterInfo::getReservedRegs(const MachineFunction &MF) const
{
  BitVector Reserved(getNumRegs());

  return Reserved;
}

void Z80RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator I,
  int SPAdj, RegScavenger *RS) const
{
  assert(0 && "Not implemented yet!");
}

unsigned Z80RegisterInfo::getFrameRegister(const MachineFunction &MF) const
{
  assert(0 && "Not implemented yet!");
  return 0;
}
