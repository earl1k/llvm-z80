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
  if (Z80::GR8RegClass.contains(DestReg, SrcReg))
  {
    // copy GR8 to GR8
    BuildMI(MBB, I, DL, get(Z80::LD8rr), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }
  else if (Z80::BR16RegClass.contains(DestReg, SrcReg))
  {
    // copy BR16 to BR16
    if (Z80::EXR16RegClass.contains(DestReg, SrcReg) && KillSrc)
    {
      BuildMI(MBB, I, DL, get(Z80::EX_DE_HL));
    }
    else
    {
      unsigned DestSubReg, SrcSubReg;

      DestSubReg = RI.getSubReg(DestReg, Z80::subreg_hi);
      SrcSubReg  = RI.getSubReg(SrcReg,  Z80::subreg_hi);
      BuildMI(MBB, I, DL, get(Z80::LD8rr), DestSubReg)
        .addReg(SrcSubReg);

      DestSubReg = RI.getSubReg(DestReg, Z80::subreg_lo);
      SrcSubReg  = RI.getSubReg(SrcReg,  Z80::subreg_lo);
      BuildMI(MBB, I, DL, get(Z80::LD8rr), DestSubReg)
        .addReg(SrcSubReg);
    }

    if (KillSrc)
      BuildMI(MBB, I, DL, get(Z80::KILL))
        .addReg(SrcReg);
    return;
  }
  else if (Z80::GR16RegClass.contains(DestReg) ||
           Z80::GR16RegClass.contains(SrcReg))
  {
    // copy GR16 to GR16
    BuildMI(MBB, I, DL, get(Z80::PUSH16r))
      .addReg(SrcReg, getKillRegState(KillSrc));
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
  case Z80::ADD8xm:
  case Z80::ADC8xm:
  case Z80::AND8xm:
  case Z80::XOR8xm:
  case Z80::OR8xm:
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
    Z80::CondCode Z80CC = static_cast<Z80::CondCode>(I->getOperand(1).getImm());
    if (Z80CC == Z80::COND_INVALID)
      return true;

    // Working from the bottom, handle the first conditional branch.
    if (Cond.empty())
    {
      FBB = TBB;
      TBB = I->getOperand(0).getMBB();
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
    .addMBB(TBB)
    .addImm(Cond[0].getImm());
  Count++;

  if (FBB)
  {
    // Two-way conditional branch. Insert the second branch.
    BuildMI(&MBB, DL, get(Z80::JP)).addMBB(FBB);
    Count++;
  }
  return Count;
}

bool Z80InstrInfo::ReverseBranchCondition(
  SmallVectorImpl<MachineOperand> &Cond) const
{
  assert(Cond.size() == 1 && "Invalid branch condition!");

  Z80::CondCode CC = static_cast<Z80::CondCode>(Cond[0].getImm());

  switch (CC)
  {
  default: return true;
  case Z80::COND_Z:
    CC = Z80::COND_NZ;
    break;
  case Z80::COND_NZ:
    CC = Z80::COND_Z;
    break;
  case Z80::COND_C:
    CC = Z80::COND_NC;
    break;
  case Z80::COND_NC:
    CC = Z80::COND_C;
    break;
  }

  Cond[0].setImm(CC);
  return false;
}

void Z80InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator MI, unsigned SrcReg, bool isKill,
  int FrameIndex, const TargetRegisterClass *RC,
  const TargetRegisterInfo *TRI) const
{
  DebugLoc dl;
  if (MI != MBB.end()) dl = MI->getDebugLoc();

  if (RC == &Z80::GR8RegClass)
    BuildMI(MBB, MI, dl, get(Z80::LD8xmr))
      .addFrameIndex(FrameIndex).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else if (RC == &Z80::BR16RegClass ||
           Z80::BR16RegClass.contains(SrcReg)) {
    BuildMI(MBB, MI, dl, get(Z80::LD16xmr))
      .addFrameIndex(FrameIndex).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  }
  else
    llvm_unreachable("Can't store this register to stack slot");
}

void Z80InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator MI, unsigned DestReg,
  int FrameIndex, const TargetRegisterClass *RC,
  const TargetRegisterInfo *TRI) const
{
  DebugLoc dl;
  if (MI != MBB.end()) dl = MI->getDebugLoc();

  if (RC == &Z80::GR8RegClass)
    BuildMI(MBB, MI, dl, get(Z80::LD8rxm), DestReg)
      .addFrameIndex(FrameIndex).addImm(0);
  else if (RC == &Z80::BR16RegClass ||
           Z80::BR16RegClass.contains(DestReg)) {
    BuildMI(MBB, MI, dl, get(Z80::LD16rxm), DestReg)
      .addFrameIndex(FrameIndex).addImm(0);
  }
  else
    llvm_unreachable("Can't load this register from stack slot");
}

bool Z80InstrInfo::expandPostRAPseudo(MachineBasicBlock::iterator MI) const
{
  MachineBasicBlock &MBB = *MI->getParent();
  MachineFunction &MF = *MBB.getParent();
  const TargetRegisterInfo &RI = *MF.getTarget().getRegisterInfo();
  DebugLoc dl = MI->getDebugLoc();
  unsigned Opc, Reg, Imm, FPReg, Idx;

  switch (MI->getOpcode())
  {
  default:
    return false;
  case Z80::LD16xmr:
    Opc   = Z80::LD8xmr;
    Reg   = MI->getOperand(2).getReg();
    FPReg = MI->getOperand(0).getReg();
    Idx   = MI->getOperand(1).getImm();
    break;
    case Z80::LD16xmi:
    Opc   = Z80::LD8xmi;
    Imm   = MI->getOperand(2).getImm();
    FPReg = MI->getOperand(0).getReg();
    Idx   = MI->getOperand(1).getImm();
    break;
  case Z80::LD16rxm:
    Opc   = Z80::LD8rxm;
    Reg   = MI->getOperand(0).getReg();
    FPReg = MI->getOperand(1).getReg();
    Idx   = MI->getOperand(2).getImm();
    break;
  }
  unsigned Lo, Hi;

  if (Opc == Z80::LD8xmi) {
    Lo = Imm & 0xFF;
    Hi = (Imm>>8) & 0xFF;
  }
  else {
    Lo = RI.getSubReg(Reg, Z80::subreg_lo);
    Hi = RI.getSubReg(Reg, Z80::subreg_hi);
  }

  switch (Opc)
  {
  case Z80::LD8xmr:
    BuildMI(MBB, MI, dl, get(Opc))
      .addReg(FPReg).addImm(Idx)
      .addReg(Lo);
    BuildMI(MBB, MI, dl, get(Opc))
      .addReg(FPReg).addImm(Idx+1)
      .addReg(Hi);
    break;
  case Z80::LD8xmi:
    BuildMI(MBB, MI, dl, get(Opc))
      .addReg(FPReg).addImm(Idx)
      .addImm(Lo);
    BuildMI(MBB, MI, dl, get(Opc))
      .addReg(FPReg).addImm(Idx+1)
      .addImm(Hi);
    break;
  case Z80::LD8rxm:
    BuildMI(MBB, MI, dl, get(Opc), Lo)
      .addReg(FPReg).addImm(Idx);
    BuildMI(MBB, MI, dl, get(Opc), Hi)
      .addReg(FPReg).addImm(Idx+1);
    break;
  }

  DEBUG(dbgs() << "Pseudo: " << *MI);
  DEBUG(dbgs() << "Replaced by:\n" <<
    "\t" << *MI->getPrevNode()->getPrevNode() <<
    "\t" << *MI->getPrevNode());

  MI->eraseFromParent();
  return true;
}
