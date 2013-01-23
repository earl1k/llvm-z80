//===-- Z80MCInstLower.h - Lower MachineInstr to MCInst ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef Z80MCINSTLOWER_H
#define Z80MCINSTLOWER_H

#include "llvm/Support/Compiler.h"

namespace llvm {
  class MCAsmInfo;
  class MCContext;
  class MCInst;
  class MCOperand;
  class MCSymbol;
  class MachineFunction;
  class MachineInstr;
  class MachineOperand;
  class Mangler;
  class TargetMachine;
  class Z80AsmPrinter;

  class Z80MCInstLower {
    MCContext &Ctx;
    Mangler *Mang;
    const MachineFunction &MF;
    const TargetMachine &TM;
    const MCAsmInfo &MAI;
    Z80AsmPrinter &AsmPrinter;
  public:
    Z80MCInstLower(Mangler *mang, const MachineFunction &mf,
      Z80AsmPrinter &asmprinter);
    void Lower(const MachineInstr *MI, MCInst &OutMI) const;

    MCOperand LowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;

    MCSymbol *GetGlobalAddressSymbol(const MachineOperand &MO) const;
    MCSymbol *GetExternalSymbolSymbol(const MachineOperand &MO) const;
  }; // end class Z80MCInstLower
} // end namespace llvm

#endif
