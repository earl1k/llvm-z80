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
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

Z80TargetLowering::Z80TargetLowering(Z80TargetMachine &TM)
  : TargetLowering(TM, new TargetLoweringObjectFileELF())
{
  addRegisterClass(MVT::i8, &Z80::GR8RegClass);
  addRegisterClass(MVT::i16, &Z80::GR16RegClass);

  computeRegisterProperties();

  setOperationAction(ISD::ZERO_EXTEND, MVT::i16, Custom);
  setOperationAction(ISD::SIGN_EXTEND, MVT::i16, Custom);

  setOperationAction(ISD::SRL, MVT::i8, Custom);
  setOperationAction(ISD::SHL, MVT::i8, Custom);
  setOperationAction(ISD::SRA, MVT::i8, Custom);
  setOperationAction(ISD::SRL, MVT::i16, Custom);
  setOperationAction(ISD::SHL, MVT::i16, Custom);
  setOperationAction(ISD::SRA, MVT::i16, Custom);

  setOperationAction(ISD::SUB,  MVT::i16, Custom);
  setOperationAction(ISD::SUBC, MVT::i16, Custom);
  setOperationAction(ISD::AND,  MVT::i16, Custom);
  setOperationAction(ISD::OR,   MVT::i16, Custom);
  setOperationAction(ISD::XOR,  MVT::i16, Custom);

  setOperationAction(ISD::SELECT_CC, MVT::i8, Custom);
}

//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "Z80GenCallingConv.inc"

SDValue Z80TargetLowering::LowerFormalArguments(SDValue Chain,
  CallingConv::ID CallConv, bool isVarArg,
  const SmallVectorImpl<ISD::InputArg> &Ins,
  DebugLoc dl, SelectionDAG &DAG,
  SmallVectorImpl<SDValue> &InVals) const
{
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &MRI = MF.getRegInfo();

  // CCValAssign - represent the assignment of
  // the arguments to a location
  SmallVector<CCValAssign, 16> ArgLocs;

  // CCState - info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
    getTargetMachine(), ArgLocs, *DAG.getContext());

  // Analyze Formal Arguments
  CCInfo.AnalyzeFormalArguments(Ins, CC_Z80);

  assert(!isVarArg && "Varargs not supported yet!");

  for (unsigned i = 0, e = ArgLocs.size(); i != e; i++)
  {
    SDValue ArgValue;
    unsigned VReg;

    CCValAssign &VA = ArgLocs[i];
    if (VA.isRegLoc())
    { // Argument passed in registers
      EVT RegVT = VA.getLocVT();
      switch (RegVT.getSimpleVT().SimpleTy)
      {
      default:
        {
#ifndef NDEBUG
          errs() << "LowerFormalArguments Unhandled argument type: "
            << RegVT.getSimpleVT().SimpleTy << "\n";
#endif
          llvm_unreachable(0);
        }
      case MVT::i8:
        VReg = MRI.createVirtualRegister(&Z80::GR8RegClass);
        MRI.addLiveIn(VA.getLocReg(), VReg);
        ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);
        InVals.push_back(ArgValue);
        break;
      case MVT::i16:
        VReg = MRI.createVirtualRegister(&Z80::GR16RegClass);
        MRI.addLiveIn(VA.getLocReg(), VReg);
        ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);
        InVals.push_back(ArgValue);
        break;
      }
    }
    else assert(0 && "Not implemented yet!");
  }
  return Chain;
}

SDValue Z80TargetLowering::LowerReturn(SDValue Chain,
  CallingConv::ID CallConv, bool isVarArg,
  const SmallVectorImpl<ISD::OutputArg> &Outs,
  const SmallVectorImpl<SDValue> &OutVals,
  DebugLoc dl, SelectionDAG &DAG) const
{
  // CCValAssign - represent the assignment of
  // the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
    getTargetMachine(), RVLocs, *DAG.getContext());

  // Analyze return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_Z80);

  // If this is the first return lowered for this function, add
  // the regs to the liveout set for the function.
  if (DAG.getMachineFunction().getRegInfo().liveout_empty()) {
    for (unsigned i = 0; i != RVLocs.size(); i++)
      if (RVLocs[i].isRegLoc())
        DAG.getMachineFunction().getRegInfo().addLiveOut(RVLocs[i].getLocReg());
  }

  SDValue Flag;

  // Copy the result value into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); i++)
  {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), OutVals[i], Flag);

    // Guarantee the all emitted copies are stuck together,
    // avoiding something bad
    Flag = Chain.getValue(1);
  }
  if (Flag.getNode())
    return DAG.getNode(Z80ISD::RET, dl, MVT::Other, Chain, Flag);
  return DAG.getNode(Z80ISD::RET, dl, MVT::Other, Chain);
}

SDValue Z80TargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
  switch (Op.getOpcode())
  {
  case ISD::ZERO_EXTEND: return LowerZExt(Op, DAG);
  case ISD::SIGN_EXTEND: return LowerSExt(Op, DAG);
  case ISD::SUB:
  case ISD::SUBC:        return LowerSUB(Op, DAG);
  case ISD::SRL:
  case ISD::SHL:
  case ISD::SRA:         return LowerShifts(Op, DAG);
  case ISD::AND:
  case ISD::OR:
  case ISD::XOR:         return LowerBinaryOp(Op, DAG);
  case ISD::SELECT_CC:   return LowerSelectCC(Op, DAG);
  default:
    llvm_unreachable("unimplemented operation");
  }
}

const char *Z80TargetLowering::getTargetNodeName(unsigned Opcode) const
{
  switch (Opcode)
  {
  default: return NULL;
  case Z80ISD::RET:       return "Z80ISD::RET";
  case Z80ISD::SCF:       return "Z80ISD::SCF";
  case Z80ISD::CCF:       return "Z80ISD::CCF";
  case Z80ISD::RLC:       return "Z80ISD::RLC";
  case Z80ISD::RRC:       return "Z80ISD::RRC";
  case Z80ISD::RL:        return "Z80ISD::RL";
  case Z80ISD::RR:        return "Z80ISD::RR";
  case Z80ISD::SLA:       return "Z80ISD::SLA";
  case Z80ISD::SRA:       return "Z80ISD::SRA";
  case Z80ISD::SLL:       return "Z80ISD::SLL";
  case Z80ISD::SRL:       return "Z80ISD::SRL";
  case Z80ISD::CP:        return "Z80ISD::CP";
  case Z80ISD::SELECT_CC: return "Z80ISD::SELECT_CC";
  case Z80ISD::BR_CC:     return "Z80ISD::BR_CC";
  }
}

SDValue Z80TargetLowering::LowerZExt(SDValue Op, SelectionDAG &DAG) const
{
  DebugLoc dl = Op.getDebugLoc();
  SDValue Val = Op.getOperand(0);
  EVT VT      = Op.getValueType();

  assert(VT == MVT::i16 && "ZExt support only i16");

  SDValue Tmp = DAG.getConstant(0, VT.getHalfSizedIntegerVT(*DAG.getContext()));
  SDValue HI  = DAG.getTargetInsertSubreg(Z80::subreg_hi, dl, VT, DAG.getUNDEF(VT), Tmp);
  SDValue LO  = DAG.getTargetInsertSubreg(Z80::subreg_lo, dl, VT, HI, Val);
  return LO;
}

SDValue Z80TargetLowering::LowerSExt(SDValue Op, SelectionDAG &DAG) const
{
  DebugLoc dl = Op.getDebugLoc();
  SDValue Val = Op.getOperand(0);
  EVT VT      = Op.getValueType();
  EVT HalfVT  = VT.getHalfSizedIntegerVT(*DAG.getContext());

  assert(VT == MVT::i16 && "SExt support only i16");

  // Generating the next code:
  // LD L,A
  // ADD A,L - set carry flag if negative (7 bit is set)
  // SBC A,A - turn to -1 if carry flag set
  // LD H,A
  SDValue LO   = DAG.getTargetInsertSubreg(Z80::subreg_lo, dl, VT, DAG.getUNDEF(VT), Val);
  SDValue Add  = DAG.getNode(ISD::ADDC, dl, DAG.getVTList(HalfVT, MVT::Glue), Val, Val);
  SDValue Flag = Add.getValue(1);
  SDValue Sub  = DAG.getNode(ISD::SUBE, dl, HalfVT, Add, Add, Flag);
  SDValue HI   = DAG.getTargetInsertSubreg(Z80::subreg_hi, dl, VT, LO, Sub);
  return HI;
}

SDValue Z80TargetLowering::LowerSUB(SDValue Op, SelectionDAG &DAG) const
{
  DebugLoc dl = Op.getDebugLoc();
  SDValue Op0 = Op.getOperand(0);
  SDValue Op1 = Op.getOperand(1);
  EVT VT      = Op.getValueType();

  assert(VT == MVT::i16 && "Only i16 SUB can by lowered");

  // Generating next code:
  // SCF, CCF - clear carry flag (FIXME: must be replaced by AND A)
  // SBC HL, $Rp - sub without carry
  SDValue Flag;
  Flag = DAG.getNode(Z80ISD::SCF, dl, MVT::Glue);
  Flag = DAG.getNode(Z80ISD::CCF, dl, MVT::Glue, Flag);
  return DAG.getNode(ISD::SUBE, dl, DAG.getVTList(VT, MVT::Glue), Op0, Op1, Flag);
}

SDValue Z80TargetLowering::LowerShifts(SDValue Op, SelectionDAG &DAG) const
{
  unsigned Opc = Op.getOpcode();
  SDNode *N    = Op.getNode();
  DebugLoc dl  = Op.getDebugLoc();
  EVT VT       = Op.getValueType();

  assert(isa<ConstantSDNode>(N->getOperand(1)) && "Not implemented yet!");

  uint64_t ShiftAmount = cast<ConstantSDNode>(N->getOperand(1))->getZExtValue();
  SDValue Victim = N->getOperand(0);

  switch (Opc)
  {
  default: llvm_unreachable("Invalid shift opcode");
  case ISD::SRL:
    Opc = Z80ISD::SRL;
    break;
  case ISD::SHL:
    Opc = Z80ISD::SLA;
    break;
  case ISD::SRA:
    Opc = Z80ISD::SRA;
    break;
  }

  if (VT == MVT::i16)
  {
    SDValue LO, HI, Flag;
    EVT HalfVT = VT.getHalfSizedIntegerVT(*DAG.getContext());
    SDVTList VTs = DAG.getVTList(HalfVT, MVT::Glue);
    LO = DAG.getTargetExtractSubreg(Z80::subreg_lo, dl, HalfVT, Victim);
    HI = DAG.getTargetExtractSubreg(Z80::subreg_hi, dl, HalfVT, Victim);
    while (ShiftAmount--) {
      if (Opc == Z80ISD::SLA) {
        LO = DAG.getNode(Opc, dl, VTs, LO);
        Flag = LO.getValue(1);
        HI = DAG.getNode(Z80ISD::RL, dl, HalfVT, HI, Flag);
      }
      else {
        HI = DAG.getNode(Opc, dl, VTs, HI);
        Flag = HI.getValue(1);
        LO = DAG.getNode(Z80ISD::RR, dl, HalfVT, LO, Flag);
      }
    }
    Victim = DAG.getTargetInsertSubreg(Z80::subreg_lo, dl, VT, DAG.getUNDEF(VT), LO);
    Victim = DAG.getTargetInsertSubreg(Z80::subreg_hi, dl, VT, Victim, HI);
  }
  else
    while (ShiftAmount--)
      Victim = DAG.getNode(Opc, dl, VT, Victim);

  return Victim;
}

SDValue Z80TargetLowering::LowerBinaryOp(SDValue Op, SelectionDAG &DAG) const
{
  unsigned Opc = Op.getOpcode();
  DebugLoc dl  = Op.getDebugLoc();
  SDValue LHS  = Op.getOperand(0);
  SDValue RHS  = Op.getOperand(1);
  EVT VT       = Op.getValueType();
  EVT HalfVT   = VT.getHalfSizedIntegerVT(*DAG.getContext());

  assert(VT == MVT::i16 && "Invalid type for lowering");

  SDValue LHS_LO, LHS_HI;
  SDValue RHS_LO, RHS_HI;
  SDValue LO, HI;
  SDValue Res;

  LHS_LO = DAG.getTargetExtractSubreg(Z80::subreg_lo, dl, HalfVT, LHS);
  LHS_HI = DAG.getTargetExtractSubreg(Z80::subreg_hi, dl, HalfVT, LHS);
  if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(RHS)) {
    RHS_LO = DAG.getConstant(CN->getZExtValue() & 0xFF, HalfVT);
    RHS_HI = DAG.getConstant(CN->getZExtValue()>>8 & 0xFF, HalfVT);
  } else {
    RHS_LO = DAG.getTargetExtractSubreg(Z80::subreg_lo, dl, HalfVT, RHS);
    RHS_HI = DAG.getTargetExtractSubreg(Z80::subreg_hi, dl, HalfVT, RHS);
  }

  LO = DAG.getNode(Opc, dl, HalfVT, LHS_LO, RHS_LO);
  HI = DAG.getNode(Opc, dl, HalfVT, LHS_HI, RHS_HI);

  Res = DAG.getTargetInsertSubreg(Z80::subreg_lo, dl, VT, DAG.getUNDEF(VT), LO);
  Res = DAG.getTargetInsertSubreg(Z80::subreg_hi, dl, VT, Res, HI);

  return Res;
}

SDValue Z80TargetLowering::EmitCMP(SDValue &LHS, SDValue &RHS, SDValue &Z80CC,
  ISD::CondCode CC, DebugLoc dl, SelectionDAG &DAG) const
{
  EVT VT = LHS.getValueType();

  assert(!VT.isFloatingPoint() && "We don't handle FP yet");
  assert((VT == MVT::i8 || VT == MVT::i16) && "Invalid type in EmitCMP");

  Z80::CondCode TCC = Z80::COND_INVALID;
  switch (CC)
  {
  case ISD::SETUNE:
  case ISD::SETNE:
    TCC = Z80::COND_NZ;
    break;
  case ISD::SETUEQ:
  case ISD::SETEQ:
    TCC = Z80::COND_Z;
    break;
  case ISD::SETUGT:
    std::swap(LHS, RHS);
  case ISD::SETULT:
    TCC = Z80::COND_C;
    break;
  case ISD::SETULE:
    std::swap(LHS, RHS);
  case ISD::SETUGE:
    TCC = Z80::COND_NC;
    break;
  default: llvm_unreachable("Invalid integer condition!");
  }
  Z80CC = DAG.getConstant(TCC, MVT::i8);

  if (VT == MVT::i8)
    return DAG.getNode(Z80ISD::CP, dl, MVT::Glue, LHS, RHS);
  else // MVT::i16
    return DAG.getNode(ISD::SUBC, dl, DAG.getVTList(VT, MVT::Glue), LHS, RHS).getValue(1);
}

SDValue Z80TargetLowering::LowerSelectCC(SDValue Op, SelectionDAG &DAG) const
{
  DebugLoc dl      = Op.getDebugLoc();
  EVT VT           = Op.getValueType();
  SDValue LHS      = Op.getOperand(0);
  SDValue RHS      = Op.getOperand(1);
  SDValue TrueV    = Op.getOperand(2);
  SDValue FalseV   = Op.getOperand(3);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();

  SDValue Z80CC;
  SDValue Flag = EmitCMP(LHS, RHS, Z80CC, CC, dl, DAG);

  return DAG.getNode(Z80ISD::SELECT_CC, dl, VT, TrueV, FalseV, Z80CC, Flag);
}

MachineBasicBlock* Z80TargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI,
  MachineBasicBlock *MBB) const
{
  unsigned Opc = MI->getOpcode();

  switch (Opc)
  {
  case Z80::SELECT8: return EmitSelectInstr(MI, MBB);
  default: llvm_unreachable("Invalid Custom Inserter Instruction");
  }
}

MachineBasicBlock* Z80TargetLowering::EmitSelectInstr(MachineInstr *MI,
  MachineBasicBlock *MBB) const
{
  DebugLoc dl = MI->getDebugLoc();
  const TargetInstrInfo &TII = *getTargetMachine().getInstrInfo();

  const BasicBlock *LLVM_BB = MBB->getBasicBlock();
  MachineFunction::iterator I = MBB;
  I++;

  MachineBasicBlock *thisMBB = MBB;
  MachineFunction *MF = MBB->getParent();
  MachineBasicBlock *copy0MBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *copy1MBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MF->insert(I, copy0MBB);
  MF->insert(I, copy1MBB);

  copy1MBB->splice(copy1MBB->begin(), MBB,
    llvm::next(MachineBasicBlock::iterator(MI)), MBB->end());
  MBB->addSuccessor(copy0MBB);
  MBB->addSuccessor(copy1MBB);

  BuildMI(MBB, dl, TII.get(Z80::JPCC))
    .addImm(MI->getOperand(3).getImm())
    .addMBB(copy1MBB);

  MBB = copy0MBB;
  MBB->addSuccessor(copy1MBB);

  MBB = copy1MBB;
  BuildMI(*MBB, MBB->begin(), dl, TII.get(Z80::PHI),
    MI->getOperand(0).getReg())
    .addReg(MI->getOperand(1).getReg()).addMBB(thisMBB)
    .addReg(MI->getOperand(2).getReg()).addMBB(copy0MBB);

  MI->eraseFromParent();
  return MBB;
}
