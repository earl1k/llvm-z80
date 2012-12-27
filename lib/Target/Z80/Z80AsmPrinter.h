//===-- Z80AsmPrinter.h - Z80 LLVM Assembly Printer -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Z80 assembly code printer class.
//
//===----------------------------------------------------------------------===//

#ifndef Z80ASMPRINTER_H
#define Z80ASMPRINTER_H

#include "Z80TargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"

namespace llvm {
  class Z80AsmPrinter : public AsmPrinter {
  public:
    explicit Z80AsmPrinter(TargetMachine &TM, MCStreamer &Streamer)
      : AsmPrinter(TM, Streamer) {}
    virtual const char *getPassName() const {
      return "Z80 Assembly Printer";
    }
    virtual void EmitInstruction(const MachineInstr *MI);
  }; // end class Z80AsmPrinter
} // end namespace llvm

#endif
