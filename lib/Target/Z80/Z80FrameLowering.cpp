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
#include "Z80InstrInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
using namespace llvm;

Z80FrameLowering::Z80FrameLowering(const Z80TargetMachine &tm)
  : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 1, -2), TM(tm)
{}

bool Z80FrameLowering::hasFP(const MachineFunction &MF) const
{
  return true;
}

void Z80FrameLowering::emitPrologue(MachineFunction &MF) const
{
  MachineBasicBlock &MBB = MF.front();  // Prolog goes into entry BB
  MachineBasicBlock::iterator MBBI = MBB.begin();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  const Z80InstrInfo &TII = 
    *static_cast<const Z80InstrInfo*>(MF.getTarget().getInstrInfo());
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  uint64_t StackSize = MFI->getStackSize();

  assert(hasFP(MF) && "Support only emitPrologue with FP");

  if (StackSize)
  {
    unsigned FP = TII.getRegisterInfo().getFrameRegister(MF);

    BuildMI(MBB, MBBI, dl, TII.get(Z80::PUSH16r))
      .addReg(FP, RegState::Kill);
    BuildMI(MBB, MBBI, dl, TII.get(Z80::LD16ri), FP)
      .addImm(-StackSize);
    BuildMI(MBB, MBBI, dl, TII.get(Z80::ADD16rSP), FP);
    BuildMI(MBB, MBBI, dl, TII.get(Z80::LD16SPr))
      .addReg(FP);
  }
}

void Z80FrameLowering::emitEpilogue(MachineFunction &MF,
  MachineBasicBlock &MBB) const
{
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  const Z80InstrInfo &TII =
    *static_cast<const Z80InstrInfo*>(MF.getTarget().getInstrInfo());

  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  unsigned RetOpcode = MBBI->getOpcode();
  DebugLoc dl = MBBI->getDebugLoc();

  assert(hasFP(MF) && "Support only emitEpilogue with FP");

  if (RetOpcode != Z80::RET)
    llvm_unreachable("Can only insert epilog into returning blocks");

  // Get the number of bytes to allocate from the FrameInfo
  uint64_t StackSize = MFI->getStackSize();

  if (StackSize)
  {
    unsigned FP = TII.getRegisterInfo().getFrameRegister(MF);

    BuildMI(MBB, MBBI, dl, TII.get(Z80::LD16ri), FP)
      .addImm(StackSize);
    BuildMI(MBB, MBBI, dl, TII.get(Z80::ADD16rSP), FP);
    BuildMI(MBB, MBBI, dl, TII.get(Z80::LD16SPr))
      .addReg(FP, RegState::Kill);
    BuildMI(MBB, MBBI, dl, TII.get(Z80::POP16r), FP);
  }
}
