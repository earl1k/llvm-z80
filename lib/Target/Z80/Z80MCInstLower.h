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
  class MachineFunction;
  class MachineInstr;
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
  }; // end class Z80MCInstLower
} // end namespace llvm

#endif
