add_public_tablegen_target(Z80CommonTableGen)

add_llvm_target(Z80CodeGen
  Z80TargetMachine.cpp
)

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
