//===-- Z80SelectionDAGInfo.h - Z80 SelectionDAG Info -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the Z80 subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef Z80SELECTIONDAGINFO_H
#define Z80SELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {
  class Z80SelectionDAGInfo : public TargetSelectionDAGInfo {
  public:
    explicit Z80SelectionDAGInfo(const Z80TargetMachine &tm);
    ~Z80SelectionDAGInfo();
  }; // end class Z80SelectionDAGInfo
} // end namespace llvm

#endif
