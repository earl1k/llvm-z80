set(LLVM_TARGET_DEFINITIONS Z80.td)

tablegen(LLVM Z80GenRegisterInfo.inc -gen-register-info)
add_public_tablegen_target(Z80CommonTableGen)

add_llvm_target(Z80CodeGen
  Z80RegisterInfo.cpp
  Z80TargetMachine.cpp
)

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
