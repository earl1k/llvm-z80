//===-- Z80AsmPrinter.cpp - Z80 LLVM Assembly Printer ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to Z80 machine code.
//
//===----------------------------------------------------------------------===//

#include "Z80AsmPrinter.h"
#include "Z80.h"
#include "Z80TargetMachine.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

void Z80AsmPrinter::EmitInstruction(const MachineInstr *MI)
{
  SmallString<128> Str;
  raw_svector_ostream OS(Str);
  printInstruction(MI, OS);
  OutStreamer.EmitRawText(OS.str());
}

#include "Z80GenAsmWriter.inc"

//===----------------------------------------------------------------------===//
// Target Registry Stuff
//===----------------------------------------------------------------------===//

extern "C" void LLVMInitializeZ80AsmPrinter()
{
  RegisterAsmPrinter<Z80AsmPrinter> X(TheZ80Target);
}
