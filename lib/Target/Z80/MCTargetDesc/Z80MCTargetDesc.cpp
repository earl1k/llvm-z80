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
#include "InstPrinter/Z80InstPrinter.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "Z80GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "Z80GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "Z80GenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createZ80MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitZ80MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createZ80MCRegisterInfo(StringRef TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitZ80MCRegisterInfo(X, Z80::PC);
  return X;
}

static MCSubtargetInfo *createZ80MCSubtargetInfo(StringRef TT, StringRef CPU,
  StringRef FS) {
    MCSubtargetInfo *X = new MCSubtargetInfo();
    InitZ80MCSubtargetInfo(X, TT, CPU, FS);
    return X;
}

static MCCodeGenInfo *createZ80MCCodeGenInfo(StringRef TT, Reloc::Model RM,
  CodeModel::Model CM, CodeGenOpt::Level OL) {
    MCCodeGenInfo *X = new MCCodeGenInfo();
    X->InitMCCodeGenInfo(RM, CM, OL);
    return X;
}

static MCInstPrinter *createZ80MCInstPrinter(const Target &T,
  unsigned SyntaxVariant, const MCAsmInfo &MAI, const MCInstrInfo &MII,
  const MCRegisterInfo &MRI, const MCSubtargetInfo &STI)
{
  return new Z80InstPrinter(MAI, MII, MRI);
}

extern "C" void LLVMInitializeZ80TargetMC() {
  // Register the MC asm info
  RegisterMCAsmInfo<Z80MCAsmInfo> X(TheZ80Target);

  // Register the MC codegen info
  TargetRegistry::RegisterMCCodeGenInfo(TheZ80Target,
    createZ80MCCodeGenInfo);

  // Register the MC instruction info
  TargetRegistry::RegisterMCInstrInfo(TheZ80Target,
    createZ80MCInstrInfo);

  // Register the MC register info
  TargetRegistry::RegisterMCRegInfo(TheZ80Target,
    createZ80MCRegisterInfo);

  // Register the MC subtarget info
  TargetRegistry::RegisterMCSubtargetInfo(TheZ80Target,
    createZ80MCSubtargetInfo);

  // Register the MCInstPrinter
  TargetRegistry::RegisterMCInstPrinter(TheZ80Target,
    createZ80MCInstPrinter);

  // Register the MCCodeEmitter
  TargetRegistry::RegisterMCCodeEmitter(TheZ80Target,
    createZ80MCCodeEmitter);

  // Register the MCAsmBackend
  TargetRegistry::RegisterMCAsmBackend(TheZ80Target,
    createZ80AsmBackend);
}
