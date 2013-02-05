//===-- Z80InstPrinter.cpp - Convert Z80 MCInst to assembly syntax --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an Z80 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "Z80InstPrinter.h"
#include "Z80.h"
#include "Z80InstrInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

// Include the auto-generated portion of the assembler writer
#include "Z80GenAsmWriter.inc"

void Z80InstPrinter::printInst(const MCInst *MI, raw_ostream &O,
  StringRef Annot)
{
  printInstruction(MI, O);
  printAnnotation(O, Annot);
}

void Z80InstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
  raw_ostream &O)
{
  const MCOperand &Op = MI->getOperand(OpNo);

  if (Op.isReg())
    O << getRegisterName(Op.getReg());
  else if (Op.isImm())
    O << Op.getImm();
  else if (Op.isExpr())
    O << *Op.getExpr();
  else assert(0 && "unknown operand kind in printOperand");
}

void Z80InstPrinter::printCCOperand(const MCInst *MI, unsigned OpNo,
  raw_ostream &O)
{
  const MCOperand &Op = MI->getOperand(OpNo);
  assert(Op.isImm() && "Invalid CC operand");

  const char *cond;

  switch (Op.getImm())
  {
  default:
    llvm_unreachable("Invalid CC operand");
  case Z80::COND_NZ:
    cond = "nz";
    break;
  case Z80::COND_Z:
    cond = "z";
    break;
  case Z80::COND_NC:
    cond = "nc";
    break;
  case Z80::COND_C:
    cond = "c";
    break;
  case Z80::COND_PO:
    cond = "po";
    break;
  case Z80::COND_PE:
    cond = "pe";
    break;
  case Z80::COND_P:
    cond = "p";
    break;
  case Z80::COND_M:
    cond = "m";
    break;
  }
  O << cond;
}
