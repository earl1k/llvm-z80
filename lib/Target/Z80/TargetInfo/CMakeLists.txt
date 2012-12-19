include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMZ80Info
  Z80TargetInfo.cpp
)

add_dependencies(LLVMZ80Info Z80CommonTableGen)
