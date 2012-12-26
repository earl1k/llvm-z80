//===-- Z80FrameLowering.cpp - Z80 Frame Information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Z80 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "Z80FrameLowering.h"
#include "Z80.h"
using namespace llvm;

Z80FrameLowering::Z80FrameLowering(const Z80TargetMachine &tm)
  : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 1, 2), TM(tm)
{}

bool Z80FrameLowering::hasFP(const MachineFunction &MF) const
{
  return false;
}

void Z80FrameLowering::emitPrologue(MachineFunction &MF) const
{
}

void Z80FrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const
{
}
