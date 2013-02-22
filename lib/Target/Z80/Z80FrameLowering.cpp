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
#include "Z80MachineFunctionInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
using namespace llvm;

Z80FrameLowering::Z80FrameLowering(const Z80TargetMachine &tm)
  : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 1, -2), TM(tm)
{}

bool Z80FrameLowering::hasFP(const MachineFunction &MF) const
{
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  return (MFI->hasCalls() || (MFI->getNumObjects() > 0));
}

void Z80FrameLowering::emitPrologue(MachineFunction &MF) const
{
  MachineBasicBlock &MBB = MF.front();  // Prolog goes into entry BB
  MachineBasicBlock::iterator MBBI = MBB.begin();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  Z80MachineFunctionInfo *Z80FI = MF.getInfo<Z80MachineFunctionInfo>();
  const Z80InstrInfo &TII = 
    *static_cast<const Z80InstrInfo*>(MF.getTarget().getInstrInfo());
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  uint64_t StackSize     = MFI->getStackSize();
  uint64_t CallFrameSize = MFI->getMaxCallFrameSize();
  uint64_t FrameSize     = Z80FI->getCalleeSavedFrameSize();

  uint64_t NumBytes = StackSize + CallFrameSize - FrameSize;

  // Skip the callee-saved push instructions.
  while (MBBI != MBB.end() && (MBBI->getOpcode() == Z80::PUSH16r))
    MBBI++;

  if (NumBytes)
  {
    unsigned FP = TII.getRegisterInfo().getFrameRegister(MF);

    BuildMI(MBB, MBBI, dl, TII.get(Z80::LD16ri), FP)
      .addImm(-NumBytes);
    BuildMI(MBB, MBBI, dl, TII.get(Z80::ADD16rSP), FP);
    BuildMI(MBB, MBBI, dl, TII.get(Z80::LD16SPr))
      .addReg(FP);
  }
}

void Z80FrameLowering::emitEpilogue(MachineFunction &MF,
  MachineBasicBlock &MBB) const
{
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  Z80MachineFunctionInfo *Z80FI = MF.getInfo<Z80MachineFunctionInfo>();
  const Z80InstrInfo &TII =
    *static_cast<const Z80InstrInfo*>(MF.getTarget().getInstrInfo());

  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  unsigned RetOpcode = MBBI->getOpcode();
  DebugLoc dl = MBBI->getDebugLoc();

  if (RetOpcode != Z80::RET)
    llvm_unreachable("Can only insert epilog into returning blocks");

  // Get the number of bytes to allocate from the FrameInfo
  uint64_t StackSize     = MFI->getStackSize();
  uint64_t CallFrameSize = MFI->getMaxCallFrameSize();
  uint64_t FrameSize     = Z80FI->getCalleeSavedFrameSize();

  uint64_t NumBytes = StackSize + CallFrameSize - FrameSize;

  // Skip the callee-saved pop instructions.
  while (MBBI != MBB.begin())
  {
    MachineBasicBlock::iterator I = prior(MBBI);
    unsigned Opc = I->getOpcode();
    if (Opc != Z80::POP16r && !I->isTerminator())
      break;
    MBBI--;
  }

  if (NumBytes)
  {
    unsigned FP = TII.getRegisterInfo().getFrameRegister(MF);

    BuildMI(MBB, MBBI, dl, TII.get(Z80::LD16ri), FP)
      .addImm(NumBytes);
    BuildMI(MBB, MBBI, dl, TII.get(Z80::ADD16rSP), FP);
    BuildMI(MBB, MBBI, dl, TII.get(Z80::LD16SPr))
      .addReg(FP, RegState::Kill);
  }
}

bool Z80FrameLowering::spillCalleeSavedRegisters(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator MI,
  const std::vector<CalleeSavedInfo> &CSI,
  const TargetRegisterInfo *TRI) const
{
  if (CSI.empty())
    return false;

  DebugLoc dl;
  if (MI != MBB.end()) dl = MI->getDebugLoc();

  MachineFunction &MF = *MBB.getParent();
  const TargetInstrInfo &TII = *MF.getTarget().getInstrInfo();
  Z80MachineFunctionInfo *MFI = MF.getInfo<Z80MachineFunctionInfo>();
  MFI->setCalleeSavedFrameSize(CSI.size() * 2);

  for (unsigned i = CSI.size(); i != 0; i--)
  {
    unsigned Reg = CSI[i-1].getReg();

    // Add the callee-saved register as live-in. It's killed at the spill.
    MBB.addLiveIn(Reg);
    BuildMI(MBB, MI, dl, TII.get(Z80::PUSH16r))
      .addReg(Reg, RegState::Kill);
  }
  return true;
}

bool Z80FrameLowering::restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator MI,
  const std::vector<CalleeSavedInfo> &CSI,
  const TargetRegisterInfo *TRI) const
{
  if (CSI.empty())
    return false;

  DebugLoc dl;
  if (MI != MBB.end()) dl = MI->getDebugLoc();

  MachineFunction &MF = *MBB.getParent();
  const TargetInstrInfo &TII = *MF.getTarget().getInstrInfo();

  for (unsigned i = 0, e = CSI.size(); i != e; i++)
    BuildMI(MBB, MI, dl, TII.get(Z80::POP16r), CSI[i].getReg());

  return true;
}

void Z80FrameLowering::processFunctionBeforeCalleeSavedScan(
  MachineFunction &MF, RegScavenger *RS) const
{
  if (hasFP(MF))
  {
    unsigned FP = MF.getTarget().getRegisterInfo()->getFrameRegister(MF);
    MF.getRegInfo().setPhysRegUsed(FP);
  }
}
