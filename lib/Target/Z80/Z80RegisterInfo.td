//===- Z80RegisterInfo.td - Describe the Z80 Register Info ----*- tblgen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Declaration that describes the Z80 register file
//===----------------------------------------------------------------------===//

let Namespace = "Z80" in {
  def A : Register<"A">;
  def PC : Register<"PC">;


  def R8 : RegisterClass<"Z80", [i8], 8, (add A)>;
}
