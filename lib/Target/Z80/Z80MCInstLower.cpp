//===-- Z80MCInstLower.cpp - Convert Z80 MachineInstr to an MCInst --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower Z80 MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "Z80MCInstLower.h"
#include "Z80AsmPrinter.h"
using namespace llvm;

Z80MCInstLower::Z80MCInstLower(Mangler *mang, const MachineFunction &mf,
  Z80AsmPrinter &asmprinter)
  : Ctx(mf.getContext()), Mang(mang), MF(mf), TM(mf.getTarget()),
  MAI(*TM.getMCAsmInfo()), AsmPrinter(asmprinter)
{}

void Z80MCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const
{
  OutMI.setOpcode(MI->getOpcode());

  for (unsigned i = 0, e = MI->getNumOperands(); i != e; i++)
  {
    const MachineOperand &MO = MI->getOperand(i);

    MCOperand MCOp;
    switch (MO.getType())
    {
    default:
      MI->dump();
      llvm_unreachable("unknown operand type");
    case MachineOperand::MO_Register:
      // Ignore all implicit register operands
      if (MO.isImplicit()) continue;
      MCOp = MCOperand::CreateReg(MO.getReg());
      break;
    case MachineOperand::MO_Immediate:
      MCOp = MCOperand::CreateImm(MO.getImm());
      break;
    }
    OutMI.addOperand(MCOp);
  }
}
