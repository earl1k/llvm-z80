##===- lib/Target/Z80/Makefile -----------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMZ80CodeGen
TARGET = Z80

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = Z80GenInstrInfo.inc Z80GenRegisterInfo.inc \
                Z80GenAsmWriter.inc Z80GenCallingConv.inc

DIRS = TargetInfo MCTargetDesc

include $(LEVEL)/Makefile.common
