//===-- Z80MCCodeEmitter.cpp - Convert Z80 code to machine code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Z80MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Z80MCTargetDesc.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
using namespace llvm;

namespace llvm {
  class Z80MCCodeEmitter : public MCCodeEmitter {
    const MCInstrInfo &MCII;
    const MCSubtargetInfo &STI;
    MCContext &Ctx;
  public:
    Z80MCCodeEmitter(const MCInstrInfo &mcii, const MCSubtargetInfo &sti,
      MCContext &ctx)
      : MCII(mcii), STI(sti), Ctx(ctx) {}

    ~Z80MCCodeEmitter() {}

    void EncodeInstruction(const MCInst &MI, raw_ostream &OS,
      SmallVectorImpl<MCFixup> &Fixups) const;
    void EmitByte(unsigned char Byte, raw_ostream &OS) const {
      OS << Byte;
    }
    void EmitInstruction(uint64_t Code, unsigned Size, raw_ostream &OS) const {
      unsigned ShiftValue = (Size * 8) - 8;
      for (unsigned i = 0; i < Size; i++) {
        EmitByte(Code >> ShiftValue, OS);
        Code >>= 8;
      }
    }

    // getBinaryCodeForInstr - tblgen generated function for getting the
    // binary encoding for an instruction.
    uint64_t getBinaryCodeForInstr(const MCInst &MI,
      SmallVectorImpl<MCFixup> &Fixups) const;
  }; // end class Z80MCCodeEmitter
} // end namespace llvm

MCCodeEmitter *llvm::createZ80MCCodeEmitter(const MCInstrInfo &MCII,
  const MCRegisterInfo &MRI, const MCSubtargetInfo &STI, MCContext &Ctx)
{
  return new Z80MCCodeEmitter(MCII, STI, Ctx);
}

void Z80MCCodeEmitter::EncodeInstruction(const MCInst &MI, raw_ostream &OS,
  SmallVectorImpl<MCFixup> &Fixups) const
{
  unsigned Opcode = MI.getOpcode();
  const MCInstrDesc &Desc = MCII.get(Opcode);
  unsigned Size = Desc.getSize();
  uint64_t Bits = getBinaryCodeForInstr(MI, Fixups);

  EmitInstruction(Bits, Size, OS);
}

#include "Z80GenMCCodeEmitter.inc"
