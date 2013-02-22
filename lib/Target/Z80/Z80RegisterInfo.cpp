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
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Support/CommandLine.h"

#define GET_REGINFO_TARGET_DESC
#include "Z80GenRegisterInfo.inc"

using namespace llvm;

typedef enum {
  IX,
  IY
} FrameUseRegister;

static cl::opt<FrameUseRegister>
FrameRegister("z80-frame-register",
  cl::desc("Frame register (IX by default)"),
  cl::init(IX),
  cl::values(
    clEnumValN(IX, "ix", "IX register"),
    clEnumValN(IY, "iy", "IY register"),
    clEnumValEnd));

Z80RegisterInfo::Z80RegisterInfo(Z80TargetMachine &tm, const TargetInstrInfo &tii)
  : Z80GenRegisterInfo(Z80::PC), TM(tm), TII(tii)
{}

const uint16_t* Z80RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const
{
  static const uint16_t CalleeSavedRegs[] = {
    Z80::BC, Z80::DE, Z80::IX, Z80::IY,
    0
  };
  return CalleeSavedRegs;
}

const uint32_t* Z80RegisterInfo::getCallPreservedMask(CallingConv::ID CallConv) const
{
  return CSR_16_RegMask;
}

BitVector Z80RegisterInfo::getReservedRegs(const MachineFunction &MF) const
{
  BitVector Reserved(getNumRegs());

  Reserved.set(Z80::PC);
  Reserved.set(Z80::SP);
  Reserved.set(Z80::FLAGS);
  Reserved.set(getFrameRegister(MF));

  return Reserved;
}

void Z80RegisterInfo::eliminateCallFramePseudoInstr(MachineFunction &MF,
  MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const
{
  MachineInstr &MI = *I;

  switch (MI.getOpcode())
  {
  default: llvm_unreachable("Cannot handle this call frame pseudo instruction");
  case Z80::ADJCALLSTACKDOWN:
  case Z80::ADJCALLSTACKUP:
    break;
  }
  MBB.erase(I);
}

void Z80RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator I,
  int SPAdj, unsigned FIOperandNum, RegScavenger *RS) const
{
  assert(SPAdj == 0 && "Not implemented yet!");

  unsigned i = 0;
  MachineInstr &MI = *I;
  MachineBasicBlock &MBB = *MI.getParent();
  MachineFunction &MF = *MBB.getParent();
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();

  while (!MI.getOperand(i).isFI())
  {
    i++;
    assert(i < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");
  }

  int FrameIndex = MI.getOperand(i).getIndex();
  uint64_t Offset = MF.getFrameInfo()->getObjectOffset(FrameIndex);
  Offset += 2; // Skip the saved PC
  Offset += MF.getFrameInfo()->getStackSize();
  Offset += MI.getOperand(i+1).getImm();

  if (TFI->hasFP(MF))
  {
    MI.getOperand(i).ChangeToRegister(getFrameRegister(MF), false);
    MI.getOperand(i+1).ChangeToImmediate(Offset);
  }
  else assert(0 && "Not implemented yet !");
}

unsigned Z80RegisterInfo::getFrameRegister(const MachineFunction &MF) const
{
  return (FrameRegister == IX) ? Z80::IX : Z80::IY;
}
