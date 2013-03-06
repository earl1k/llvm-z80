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
    enum Prefixes {
      NoPrefix = 0,
      DD = 1,
      FD = 2,
      RegPrefixMask = 3,
      ED = 3,
      CB = 4,
      DDCB = 5,
      FDCB = 6,
      PrefixMask = 7
    }; // end enum
    inline unsigned getPrefix(uint64_t TSFlags)
    {
      switch (TSFlags & PrefixMask)
      {
      default: llvm_unreachable("Invalid Prefix");
      case NoPrefix: return 0x00;
      case CB:       return 0xCB;
      case ED:       return 0xED;
      case DD:       return 0xDD;
      case FD:       return 0xFD;
      case DDCB:     return 0xCBDD;
      case FDCB:     return 0xCBFD;
      }
    }
    inline void setRegPrefix(uint64_t &TSFlags, Prefixes Prefix)
    {
      switch (Prefix)
      {
      default: llvm_unreachable("Invalid Prefix");
      case NoPrefix:
        return;
      case FD:
      case DD:
        break;
      }
      switch (TSFlags & PrefixMask)
      {
      default: llvm_unreachable("Invalid Prefix");
      case NoPrefix:
      case CB:
        break;
      }
      TSFlags += Prefix;
    }
  } // end namespace Z80II
} // end namespace llvm

#endif
