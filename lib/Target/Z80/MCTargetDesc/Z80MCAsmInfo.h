//===-- Z80MCAsmInfo.h - Z80 asm properties --------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the Z80MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef Z80TARGETASMINFO_H
#define Z80TARGETASMINFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {
  class StringRef;
  class Target;

  class Z80MCAsmInfo : public MCAsmInfo {
  public:
    explicit Z80MCAsmInfo(const Target &T, StringRef TT);
  }; // end class Z80MCAsmInfo
} // end namespace llvm

#endif
