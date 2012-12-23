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

#define GET_INSTRINFO_CTOR
#include "Z80GenInstrInfo.inc"

using namespace llvm;

Z80InstrInfo::Z80InstrInfo(Z80TargetMachine &tm)
  : RI(tm, *this), TM(tm)
{}
