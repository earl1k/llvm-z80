//===- Z80ISelDAGToDAG.cpp - A DAG pattern matching inst selector for Z80 -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a DAG pattern matching instruction selector for Z80,
// converting from a legalized dag to a Z80 dag.
//
//===----------------------------------------------------------------------===//

#include "Z80.h"
#include "Z80TargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

namespace {
  class Z80DAGToDAGISel : public SelectionDAGISel {
  public:
    Z80DAGToDAGISel(Z80TargetMachine &TM, CodeGenOpt::Level OptLevel)
      : SelectionDAGISel(TM, OptLevel)
    {}
#include "Z80GenDAGISel.inc"
  private:
    SDNode *Select(SDNode *N);
    bool SelectXAddr(SDValue N, SDValue &Base, SDValue &Disp);
  }; // end class Z80DAGToDAGISel
} // end namespace

// createZ80ISelDAG - This pass converts a legalized DAG into a
// Z80-specific DAG, ready for instruction scheduling.

FunctionPass *llvm::createZ80ISelDAG(Z80TargetMachine &TM,
  CodeGenOpt::Level OptLevel) {
  return new Z80DAGToDAGISel(TM, OptLevel);
}

SDNode *Z80DAGToDAGISel::Select(SDNode *Node)
{
  DebugLoc dl = Node->getDebugLoc();

  // Dump information about the Node being selected
  DEBUG(errs() << "Selecting: ";
    Node->dump(CurDAG);
    errs() << "\n");

  // If we have a custom node, we already have selected
  if (Node->isMachineOpcode()) {
    DEBUG(errs() << "== ";
      Node->dump(CurDAG);
      errs() << "\n");
    return NULL;
  }

  switch (Node->getOpcode())
  {
  default: break;
  }

  // Select the default instruction
  SDNode *ResNode = SelectCode(Node);

  if (ResNode == NULL || ResNode == Node)
    DEBUG(Node->dump(CurDAG));
  else
    DEBUG(errs() << "\n");

  return ResNode;
}

bool Z80DAGToDAGISel::SelectXAddr(SDValue N, SDValue &Base, SDValue &Disp)
{
  switch (N->getOpcode())
  {
  case ISD::FrameIndex:
    FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(N);
    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i16);
    Disp = CurDAG->getTargetConstant(0, MVT::i8);
    return true;
  }
  return false;
}
