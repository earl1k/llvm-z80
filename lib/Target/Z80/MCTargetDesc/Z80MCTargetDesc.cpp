//===-- Z80MCTargetDesc.cpp - Z80 Target Descriptions -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Z80 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "Z80MCTargetDesc.h"
#include "Z80MCAsmInfo.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_REGINFO_MC_DESC
#include "Z80GenRegisterInfo.inc"

#define GET_INSTRINFO_MC_DESC
#include "Z80GenInstrInfo.inc"

using namespace llvm;

extern "C" void LLVMInitializeZ80TargetMC() {
  // Register the MC asm info
  RegisterMCAsmInfo<Z80MCAsmInfo> X(TheZ80Target);
}
