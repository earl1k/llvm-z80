//===-- Z80ISelLowering.cpp - X86 DAG Lowering Implementation -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Z80 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#include "Z80ISelLowering.h"
#include "Z80.h"
#include "Z80TargetMachine.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
using namespace llvm;

Z80TargetLowering::Z80TargetLowering(Z80TargetMachine &TM)
  : TargetLowering(TM, new TargetLoweringObjectFileELF())
{
}

SDValue Z80TargetLowering::LowerFormalArguments(SDValue Chain,
  CallingConv::ID CallConv, bool isVarArg,
  const SmallVectorImpl<ISD::InputArg> &Ins,
  DebugLoc dl, SelectionDAG &DAG,
  SmallVectorImpl<SDValue> &InVals) const
{
  return Chain;
}

SDValue Z80TargetLowering::LowerReturn(SDValue Chain,
  CallingConv::ID CallConv, bool isVarArg,
  const SmallVectorImpl<ISD::OutputArg> &Outs,
  const SmallVectorImpl<SDValue> &OutVals,
  DebugLoc dl, SelectionDAG &DAG) const
{
  return DAG.getNode(Z80ISD::RET, dl, MVT::Other, Chain);
}
