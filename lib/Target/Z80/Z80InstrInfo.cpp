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
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

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
  else if (Z80::GR16RegClass.contains(DestReg, SrcReg))
  {
    unsigned DestSubReg, SrcSubReg;

    DestSubReg = RI.getSubReg(DestReg, Z80::subreg_lo);
    SrcSubReg  = RI.getSubReg(SrcReg,  Z80::subreg_lo);
    BuildMI(MBB, I, DL, get(Z80::LD8rr), DestSubReg)
      .addReg(SrcSubReg, getKillRegState(KillSrc));

    DestSubReg = RI.getSubReg(DestReg, Z80::subreg_hi);
    SrcSubReg  = RI.getSubReg(SrcReg,  Z80::subreg_hi);
    BuildMI(MBB, I, DL, get(Z80::LD8rr), DestSubReg)
      .addReg(SrcSubReg, getKillRegState(KillSrc));
  }
  else llvm_unreachable("Imposible reg-to-reg copy");
}

MachineInstr *Z80InstrInfo::commuteInstruction(MachineInstr *MI,
  bool NewMI) const
{
  switch (MI->getOpcode())
  {
  default: return TargetInstrInfo::commuteInstruction(MI, NewMI);
  case Z80::ADD8r:
  case Z80::ADD16r:
  case Z80::ADC8r:
  case Z80::ADC16r:
  case Z80::AND8r:
  case Z80::XOR8r:
  case Z80::OR8r:
    break;
  case Z80::ADD8i:
  case Z80::ADC8i:
  case Z80::AND8i:
  case Z80::XOR8i:
  case Z80::OR8i:
    return NULL;
  }
  assert(!NewMI && "Not implemented yet!");

  MachineBasicBlock &MBB = *MI->getParent();
  MachineFunction &MF = *MBB.getParent();
  unsigned reg[2], arg[] = { 0, 0 };

  MachineInstr *MILoadReg = MI->getPrevNode();
  MachineOperand &MO0 = MI->getOperand(0);
  MachineOperand &MO1 = MILoadReg->getOperand(1);
  reg[0] = MO0.getReg();
  reg[1] = MO1.getReg();

  DEBUG(dbgs() << "COMMUTING:\n\t" << *MILoadReg << "\t" << *MI);
  DEBUG(dbgs() << "COMMUTING OPERANDS: " << MO0 << ", " << MO1 << "\n");
  unsigned PreferArg = -1;

  for (MachineFunction::iterator MFI = MF.begin(), MFE = MF.end(); MFI != MFE; MFI++)
  {
    MachineBasicBlock::iterator MBBI = MFI->begin();
    while (MBBI != MFI->end())
    {
      if (MBBI->getOpcode() == TargetOpcode::COPY)
      {
        if (MBBI->findRegisterDefOperand(reg[0])) {
          DEBUG(dbgs() << "DEFINE OPERAND " << MO0 << ":\n\t" << *MBBI);
          arg[0] = MBBI->getOperand(1).getReg();
          if (RI.isPhysicalRegister(arg[0])) PreferArg = 0;
        }
        if (MBBI->findRegisterDefOperand(reg[1])) {
          DEBUG(dbgs() << "DEFINE OPERAND " << MO1 << ":\n\t" << *MBBI);
          arg[1] = MBBI->getOperand(1).getReg();
          if (RI.isPhysicalRegister(arg[0])) PreferArg = 1;
        }
        if (arg[0] && arg[1]) break;
      }
      MBBI++;
    }
    if (arg[0] && arg[1]) break;
  }

  if (arg[0] == 0 || arg[1] == 0)
  {
    DEBUG(dbgs() << "COPY TO OPERANDS NOT FOUND\n");
    return NULL;
  }

  if (PreferArg == 0)
  {
    MO0.setReg(reg[1]);
    MO1.setReg(reg[0]);
    DEBUG(dbgs() << "COMMUTING TO:\n\t" << *MILoadReg << "\t" << *MI);
  }
  else DEBUG(dbgs() << "COMMUTING NOT NEEDED\n");
  return NULL;
}
