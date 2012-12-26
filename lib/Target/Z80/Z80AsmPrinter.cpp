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
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

//===----------------------------------------------------------------------===//
// Target Registry Stuff
//===----------------------------------------------------------------------===//

extern "C" void LLVMInitializeZ80AsmPrinter()
{
  RegisterAsmPrinter<Z80AsmPrinter> X(TheZ80Target);
}
