set(LLVM_TARGET_DEFINITIONS Z80.td)

tablegen(LLVM Z80GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM Z80GenCallingConv.inc -gen-callingconv)
tablegen(LLVM Z80GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM Z80GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM Z80GenMCCodeEmitter.inc -gen-emitter -mc-emitter)
tablegen(LLVM Z80GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM Z80GenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(Z80CommonTableGen)

add_llvm_target(Z80CodeGen
  Z80AsmPrinter.cpp
  Z80FrameLowering.cpp
  Z80ISelDAGToDAG.cpp
  Z80ISelLowering.cpp
  Z80InstrInfo.cpp
  Z80MachineFunctionInfo.cpp
  Z80MCInstLower.cpp
  Z80RegisterInfo.cpp
  Z80SelectionDAGInfo.cpp
  Z80Subtarget.cpp
  Z80TargetMachine.cpp
)

add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
