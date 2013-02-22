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
  : Z80GenInstrInfo(Z80::ADJCALLSTACKDOWN, Z80::ADJCALLSTACKUP),
  RI(tm, *this), TM(tm)
{}

void Z80InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator I, DebugLoc DL,
  unsigned DestReg, unsigned SrcReg, bool KillSrc) const
{
  if (Z80::BR8RegClass.contains(DestReg, SrcReg))
  {
    BuildMI(MBB, I, DL, get(Z80::LD8rr), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }
  else if (Z80::GR8RegClass.contains(DestReg, SrcReg))
  {
    if (Z80::BR8RegClass.contains(DestReg))
    {
      // DestReg - BR8
      // SrcReg  - GR8
      if (DestReg == Z80::H || DestReg == Z80::L)
      {
        unsigned Idx = RI.getSubRegIndex(Z80::HL, DestReg);

        BuildMI(MBB, I, DL, get(Z80::EX_DE_HL));
        BuildMI(MBB, I, DL, get(Z80::LD8rr), RI.getSubReg(Z80::DE, Idx))
          .addReg(SrcReg, getKillRegState(KillSrc));
        BuildMI(MBB, I, DL, get(Z80::EX_DE_HL));
      }
      else
        BuildMI(MBB, I, DL, get(Z80::LD8rr), DestReg)
          .addReg(SrcReg, getKillRegState(KillSrc));
      return;
    }
    else if (Z80::BR8RegClass.contains(SrcReg))
    {
      // DestReg - GR8
      // SrcReg  - BR8
      if (SrcReg == Z80::H || SrcReg == Z80::L)
      {
        unsigned Idx = RI.getSubRegIndex(Z80::HL, SrcReg);

        BuildMI(MBB, I, DL, get(Z80::EX_DE_HL));
        BuildMI(MBB, I, DL, get(Z80::LD8rr), DestReg)
          .addReg(RI.getSubReg(Z80::DE, Idx), getKillRegState(KillSrc));
        BuildMI(MBB, I, DL, get(Z80::EX_DE_HL));
      }
      else
        BuildMI(MBB, I, DL, get(Z80::LD8rr), DestReg)
          .addReg(SrcReg, getKillRegState(KillSrc));
      return;
    }
    else
    {
      // DestReg - GR8
      // SrcReg  - GR8
      if ((RI.getSubRegIndex(Z80::IX, DestReg) &&
          RI.getSubRegIndex(Z80::IX, SrcReg)) ||
          (RI.getSubRegIndex(Z80::IY, DestReg) &&
          RI.getSubRegIndex(Z80::IY, SrcReg)))
      {
        BuildMI(MBB, I, DL, get(Z80::LD8rr), DestReg)
          .addReg(SrcReg, getKillRegState(KillSrc));
        return;
      }
      // copy from/to different index register
    }
  }
  else if (Z80::BR16RegClass.contains(DestReg, SrcReg))
  {
    if ((DestReg == Z80::HL || DestReg == Z80::DE) &&
        (SrcReg  == Z80::HL || SrcReg  == Z80::DE) &&
        KillSrc)
    {
      BuildMI(MBB, I, DL, get(Z80::EX_DE_HL));
      BuildMI(MBB, I, DL, get(Z80::KILL))
        .addReg(SrcReg, getKillRegState(KillSrc));
    }
    else
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
    return;
  }
  else if (Z80::GR16RegClass.contains(DestReg, SrcReg))
  {
    // use stack for copy registers
    BuildMI(MBB, I, DL, get(Z80::PUSH16r)).addReg(SrcReg);
    BuildMI(MBB, I, DL, get(Z80::POP16r), DestReg);
    return;
  }
  llvm_unreachable("Imposible reg-to-reg copy");
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
  if (MILoadReg == NULL || MILoadReg->getOpcode() != Z80::COPY) return NULL;

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

bool Z80InstrInfo::AnalyzeBranch(MachineBasicBlock &MBB,
  MachineBasicBlock *&TBB, MachineBasicBlock *&FBB,
  SmallVectorImpl<MachineOperand> &Cond, bool AllowModify = false) const
{
  // Start from the bottom of the block and work up, examining the
  // terminator instructions.
  MachineBasicBlock::iterator I = MBB.end();

  while (I != MBB.begin())
  {
    I--;
    if (I->isDebugValue())
      continue;

    // Working from the bottom, when we see a non-terminator
    // instruction, we're done.
    if (!isUnpredicatedTerminator(I))
      break;

    // A terminator that isn't a branch can't easily be handled
    // by this analisys.
    if (!I->isBranch())
      return true;

    // Handle unconditional branches.
    if (I->getOpcode() == Z80::JP)
    {
      if (!AllowModify)
      {
        TBB = I->getOperand(0).getMBB();
        continue;
      }

      // If the block has any instructions after a JP, delete them.
      while (llvm::next(I) != MBB.end())
        llvm::next(I)->eraseFromParent();

      Cond.clear();
      FBB = 0;

      // Delete the JP if it's equivalent to a fall-through.
      if (MBB.isLayoutSuccessor(I->getOperand(0).getMBB()))
      {
        TBB = 0;
        I->eraseFromParent();
        I = MBB.end();
        continue;
      }
      // TBB is used to indicate the unconditional destination.
      TBB = I->getOperand(0).getMBB();
      continue;
    }

    // Handle conditional branches.
    assert(I->getOpcode() == Z80::JPCC && "Invalid conditional branch");
    Z80::CondCode Z80CC = static_cast<Z80::CondCode>(I->getOperand(0).getImm());
    if (Z80CC == Z80::COND_INVALID)
      return true;

    // Working from the bottom, handle the first conditional branch.
    if (Cond.empty())
    {
      FBB = TBB;
      TBB = I->getOperand(1).getMBB();
      Cond.push_back(MachineOperand::CreateImm(Z80CC));
      continue;
    }

    // Handle subsequent conditional branches.
    assert(0 && "Not implemented yet!");
  }
  return false;
}

unsigned Z80InstrInfo::RemoveBranch(MachineBasicBlock &MBB) const
{
  MachineBasicBlock::iterator I = MBB.end();
  unsigned Count = 0;

  while (I != MBB.begin())
  {
    I--;
    if (I->isDebugValue())
      continue;
    if (I->getOpcode() != Z80::JP &&
        I->getOpcode() != Z80::JPCC)
        break;
    // Remove branch.
    I->eraseFromParent();
    I = MBB.end();
    Count++;
  }
  return Count;
}

unsigned Z80InstrInfo::InsertBranch(MachineBasicBlock &MBB,
  MachineBasicBlock *TBB, MachineBasicBlock *FBB,
  const SmallVectorImpl<MachineOperand> &Cond,
  DebugLoc DL) const
{
  // Shouldn't be a fall through.
  assert(TBB && "InsertBranch must not be told to insert a fallthrough");
  assert((Cond.size() == 0 || Cond.size() == 1) &&
    "Z80 branch conditions must have one component!");

  if (Cond.empty())
  {
    // Unconditional branch?
    assert(!FBB && "Unconditional branch with multiple successors!");
    BuildMI(&MBB, DL, get(Z80::JP)).addMBB(TBB);
    return 1;
  }

  // Conditional branch.
  unsigned Count = 0;
  BuildMI(&MBB, DL, get(Z80::JPCC))
    .addImm(Cond[0].getImm())
    .addMBB(TBB);
  Count++;

  if (FBB)
  {
    // Two-way conditional branch. Insert the second branch.
    BuildMI(&MBB, DL, get(Z80::JP)).addMBB(FBB);
    Count++;
  }
  return Count;
}
