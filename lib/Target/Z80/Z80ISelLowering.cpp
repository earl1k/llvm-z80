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

  setStackPointerRegisterToSaveRestore(Z80::SP);

  setBooleanContents(ZeroOrOneBooleanContent);

  setLoadExtAction(ISD::EXTLOAD, MVT::i8, Expand);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i8, Expand);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i8, Expand);

  setTruncStoreAction(MVT::i16, MVT::i8, Expand);

  setOperationAction(ISD::LOAD,  MVT::i16, Custom);
  setOperationAction(ISD::STORE, MVT::i16, Custom);

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
  setOperationAction(ISD::SELECT_CC, MVT::i16, Custom);
  setOperationAction(ISD::BR_CC, MVT::i8, Custom);
  setOperationAction(ISD::BR_CC, MVT::i16, Custom);

  setOperationAction(ISD::GlobalAddress, MVT::i16, Custom);
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

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result value into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); i++)
  {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), OutVals[i], Flag);

    // Guarantee the all emitted copies are stuck together,
    // avoiding something bad
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain;  // Update chain.

  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(Z80ISD::RET, dl, MVT::Other, &RetOps[0], RetOps.size());
}

SDValue Z80TargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
  SmallVectorImpl<SDValue> &InVals) const
{
  SelectionDAG &DAG                     = CLI.DAG;
  DebugLoc dl                           = CLI.DL;
  SmallVector<ISD::OutputArg, 32> &Outs = CLI.Outs;
  SmallVector<SDValue, 32> OutVals      = CLI.OutVals;
  SmallVector<ISD::InputArg, 32> &Ins   = CLI.Ins;
  SDValue Chain                         = CLI.Chain;
  SDValue Callee                        = CLI.Callee;
  bool isTailCall                       = CLI.IsTailCall;
  CallingConv::ID CallConv              = CLI.CallConv;
  bool isVarArg                         = CLI.IsVarArg;

  MachineFunction &MF = DAG.getMachineFunction();
  // Z80 target does not yet support tail call optimization
  isTailCall = false;

  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
    getTargetMachine(), ArgLocs, *DAG.getContext());

  CCInfo.AnalyzeCallOperands(Outs, CC_Z80);

  // Get a count of how many bytes are to be pushed on the stack
  unsigned NumBytes = CCInfo.getNextStackOffset();

  Chain = DAG.getCALLSEQ_START(Chain, DAG.getConstant(NumBytes,
    getPointerTy(), true));

  SmallVector<std::pair<unsigned, SDValue>, 4> RegsToPass;
  SmallVector<SDValue, 12> MemOpChains;
  SDValue StackPtr;

  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; i++)
  {
    CCValAssign &VA = ArgLocs[i];

    SDValue Arg = OutVals[i];
    switch (VA.getLocInfo())
    {
    default: llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full: break;
    case CCValAssign::SExt:
      Arg = DAG.getNode(ISD::SIGN_EXTEND, dl, VA.getLocVT(), Arg);
      break;
    case CCValAssign::ZExt:
      Arg = DAG.getNode(ISD::ZERO_EXTEND, dl, VA.getLocVT(), Arg);
      break;
    case CCValAssign::AExt:
      Arg = DAG.getNode(ISD::ANY_EXTEND, dl, VA.getLocVT(), Arg);
      break;
    }

    // Arguments that can be passed on register must be kept at RegsToPass
    // vector.
    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
    }
    else
    {
      assert(VA.isMemLoc());

      unsigned FP = MF.getTarget().getRegisterInfo()->getFrameRegister(MF);

      if (StackPtr.getNode() == 0)
        StackPtr = DAG.getCopyFromReg(Chain, dl, FP, getPointerTy());

      SDValue PtrOff = DAG.getNode(ISD::ADD, dl, getPointerTy(),
        StackPtr, DAG.getIntPtrConstant(VA.getLocMemOffset()));

      SDValue MemOp;
      ISD::ArgFlagsTy Flags = Outs[i].Flags;

      if (Flags.isByVal()) assert(0 && "Not implemented yet!");

      MemOp = DAG.getStore(Chain, dl, Arg, PtrOff, MachinePointerInfo(),
        false, false, 0);

      MemOpChains.push_back(MemOp);
    }
  }

  // Transform all store nodes into one single node because all store nodes are
  // independent of each other.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other,
                        &MemOpChains[0], MemOpChains.size());

  // Build a sequence of copy-to-reg nodes chained together with token chain and
  // flag operands which copy the outgoing args into registers. The Flag is
  // necessary since all emitted instructions must be stuck together.
  SDValue Flag;

  for (unsigned i = 0, e = RegsToPass.size(); i != e; i++)
  {
    Chain = DAG.getCopyToReg(Chain, dl, RegsToPass[i].first,
      RegsToPass[i].second, Flag);
    Flag = Chain.getValue(1);
  }

  // If the callee is a GlobalAddress node (quite common, every direct call is)
  // turn it into a TargetGlobalAddress node so that legalize doesn't hack it.
  // Likewise ExternalSymbol -> TargetExternalSymbol.
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl, MVT::i16);
  else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i16);

  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add a register mask operand representing the call-preserved registers.
  const TargetRegisterInfo *TRI = getTargetMachine().getRegisterInfo();
  const uint32_t *Mask = TRI->getCallPreservedMask(CallConv);
  Ops.push_back(DAG.getRegisterMask(Mask));

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (unsigned i = 0, e = RegsToPass.size(); i != e; i++)
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));

  if (Flag.getNode())
    Ops.push_back(Flag);

  Chain = DAG.getNode(Z80ISD::CALL, dl, NodeTys, &Ops[0], Ops.size());
  Flag = Chain.getValue(1);

  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(Chain,
    DAG.getConstant(NumBytes, getPointerTy(), true),
    DAG.getConstant(0, getPointerTy(), true),
    Flag);

  Flag = Chain.getValue(1);

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, Flag, CallConv, isVarArg, Ins, dl, DAG, InVals);
}

SDValue Z80TargetLowering::LowerCallResult(SDValue Chain, SDValue Flag,
  CallingConv::ID CallConv, bool isVarArg,
  const SmallVectorImpl<ISD::InputArg> &Ins,
  DebugLoc dl, SelectionDAG &DAG,
  SmallVectorImpl<SDValue> &InVals) const
{
  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
    getTargetMachine(), RVLocs, *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_Z80);

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0, e = Ins.size(); i != e; i++)
  {
    Chain = DAG.getCopyFromReg(Chain, dl, RVLocs[i].getLocReg(),
      RVLocs[i].getValVT(), Flag).getValue(1);
    Flag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }
  return Chain;
}

//===----------------------------------------------------------------------===//
//                      Custom Lowering Implementation
//===----------------------------------------------------------------------===//

SDValue Z80TargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
  switch (Op.getOpcode())
  {
  case ISD::ZERO_EXTEND:   return LowerZExt(Op, DAG);
  case ISD::SIGN_EXTEND:   return LowerSExt(Op, DAG);
  case ISD::SUB:
  case ISD::SUBC:          return LowerSUB(Op, DAG);
  case ISD::SRL:
  case ISD::SHL:
  case ISD::SRA:           return LowerShifts(Op, DAG);
  case ISD::AND:
  case ISD::OR:
  case ISD::XOR:           return LowerBinaryOp(Op, DAG);
  case ISD::SELECT_CC:     return LowerSelectCC(Op, DAG);
  case ISD::BR_CC:         return LowerBrCC(Op, DAG);
  case ISD::GlobalAddress: return LowerGlobalAddress(Op, DAG);
  case ISD::STORE:         return LowerStore(Op, DAG);
  case ISD::LOAD:          return LowerLoad(Op, DAG);
  default:
    llvm_unreachable("unimplemented operation");
  }
}

const char *Z80TargetLowering::getTargetNodeName(unsigned Opcode) const
{
  switch (Opcode)
  {
  default: return NULL;
  case Z80ISD::WRAPPER:   return "Z80ISD::WRAPPER";
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
  case Z80ISD::CALL:      return "Z80ISD::CALL";
  case Z80ISD::RET:       return "Z80ISD::RET";
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

SDValue Z80TargetLowering::LowerBrCC(SDValue Op, SelectionDAG &DAG) const
{
  DebugLoc dl      = Op.getDebugLoc();
  EVT VT           = Op.getValueType();
  SDValue Chain    = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS      = Op.getOperand(2);
  SDValue RHS      = Op.getOperand(3);
  SDValue Dest     = Op.getOperand(4);

  SDValue Z80CC;
  SDValue Flag = EmitCMP(LHS, RHS, Z80CC, CC, dl, DAG);

  return DAG.getNode(Z80ISD::BR_CC, dl, VT, Chain, Z80CC, Dest, Flag);
}

SDValue Z80TargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const
{
  DebugLoc dl = Op.getDebugLoc();
  EVT VT      = getPointerTy();

  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  int64_t Offset = cast<GlobalAddressSDNode>(Op)->getOffset();

  // Create the TargetGlobalAddress node, folding in the constant offset.
  SDValue Result = DAG.getTargetGlobalAddress(GV, dl, VT, Offset);

  return DAG.getNode(Z80ISD::WRAPPER, dl, VT, Result);
}

SDValue Z80TargetLowering::LowerStore(SDValue Op, SelectionDAG &DAG) const
{
  StoreSDNode *ST = dyn_cast<StoreSDNode>(Op);
  DebugLoc dl     = Op.getDebugLoc();
  SDValue Chain   = ST->getChain();
  SDValue BasePtr = ST->getBasePtr();
  SDValue Value   = ST->getOperand(1);

  assert(!ST->isTruncatingStore() && "Truncating Store isn't supported yet!");
  assert(!ST->isIndexed() && "Indexed Store isn't supported yet!");
  
  switch (BasePtr.getOpcode())
  {
  default: llvm_unreachable("Invalid store operand");
  case ISD::FrameIndex:
  case ISD::CopyFromReg:
    break;
  }

  SDValue Lo, Hi;
  if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Value)) {
    Lo = DAG.getConstant(CN->getZExtValue() & 0xFF, MVT::i8);
    Hi = DAG.getConstant((CN->getZExtValue()>>8) & 0xFF, MVT::i8);
  }
  else if (Value.getOpcode() == ISD::CopyFromReg) {
    if (RegisterSDNode *RN = dyn_cast<RegisterSDNode>(Value.getOperand(1))) {
      unsigned Reg = RN->getReg();
      Lo = DAG.getTargetExtractSubreg(Z80::subreg_lo, dl, MVT::i8, Value);
      Hi = DAG.getTargetExtractSubreg(Z80::subreg_hi, dl, MVT::i8, Value);
    }
  }
  else assert(0 && "Not implemented yet!");
  SDValue StoreLow = DAG.getStore(Chain, dl, Lo, BasePtr,
    ST->getPointerInfo(), ST->isVolatile(),
    ST->isNonTemporal(), ST->getAlignment());
  SDValue HighAddr = DAG.getNode(ISD::ADD, dl, MVT::i16, BasePtr,
    DAG.getConstant(1, MVT::i16));
  SDValue StoreHigh = DAG.getStore(Chain, dl, Hi, HighAddr,
    ST->getPointerInfo(), ST->isVolatile(),
    ST->isNonTemporal(), ST->getAlignment());
  return DAG.getNode(ISD::TokenFactor, dl, MVT::Other, StoreLow, StoreHigh);
}

SDValue Z80TargetLowering::LowerLoad(SDValue Op, SelectionDAG &DAG) const
{
  LoadSDNode *LD  = cast<LoadSDNode>(Op);
  DebugLoc dl     = LD->getDebugLoc();
  SDValue Chain   = LD->getChain();
  SDValue BasePtr = LD->getBasePtr();

  assert(!LD->isIndexed() && "Indexed load isn't supported yet!");
  assert(LD->getExtensionType() == ISD::NON_EXTLOAD && "Extload isn't supported yet!");

  switch (BasePtr.getOpcode())
  {
  default: llvm_unreachable("invalid load operand");
  case ISD::CopyFromReg:
  case ISD::FrameIndex:
    break;
  }
  SDValue Lo = DAG.getLoad(MVT::i8, dl, Chain, BasePtr,
    MachinePointerInfo(), LD->isVolatile(), LD->isNonTemporal(),
    LD->isInvariant(), LD->getAlignment());
  SDValue HighAddr = DAG.getNode(ISD::ADD, dl, MVT::i16, BasePtr,
    DAG.getConstant(1, MVT::i16));
  SDValue Hi = DAG.getLoad(MVT::i8, dl, Chain, HighAddr,
    MachinePointerInfo(), LD->isVolatile(), LD->isNonTemporal(),
    LD->isInvariant(), LD->getAlignment());

  SDValue LoRes = DAG.getTargetInsertSubreg(Z80::subreg_lo, dl,
    MVT::i16, DAG.getUNDEF(MVT::i16), Lo);
  SDValue HiRes = DAG.getTargetInsertSubreg(Z80::subreg_hi, dl,
    MVT::i16, LoRes, Hi);

  return HiRes;
}

//===----------------------------------------------------------------------===//
//                   Instructions With Custom Inserter
//===----------------------------------------------------------------------===//

MachineBasicBlock* Z80TargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI,
  MachineBasicBlock *MBB) const
{
  unsigned Opc = MI->getOpcode();

  switch (Opc)
  {
  case Z80::SELECT8:
  case Z80::SELECT16: return EmitSelectInstr(MI, MBB);
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
