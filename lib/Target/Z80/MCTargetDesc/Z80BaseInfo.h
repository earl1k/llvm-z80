//===-- Z80BaseInfo.h - Top level definitions for Z80 -------- --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the Z80 target useful for the compiler back-end and the MC libraries.
//
//===----------------------------------------------------------------------===//

#ifndef Z80BASEINFO_H
#define Z80BASEINFO_H

#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {
  namespace Z80II {
    enum {
      NoPrefix = 0,
      CB = 1,
      ED = 2,
      DD = 3,
      FD = 4,
      DDCB = 5,
      FDCB = 6,
      PrefixMask = 7,
    }; // end enum
    inline unsigned char getPrefix(uint64_t TSFlags)
    {
      switch (TSFlags & PrefixMask)
      {
      default: assert(0 && "Invalid Prefix");
      case CB: return 0xCB;
      case ED: return 0xED;
      case DD: return 0xDD;
      case FD: return 0xFD;
      }
    }
  } // end namespace Z80II
} // end namespace llvm

#endif
