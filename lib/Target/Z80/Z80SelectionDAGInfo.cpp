//===-- Z80SelectionDAGInfo.cpp - Z80 SelectionDAG Info -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Z80SelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#include "Z80TargetMachine.h"
using namespace llvm;

Z80SelectionDAGInfo::Z80SelectionDAGInfo(const Z80TargetMachine &tm)
  : TargetSelectionDAGInfo(tm)
{
}

Z80SelectionDAGInfo::~Z80SelectionDAGInfo()
{
}
