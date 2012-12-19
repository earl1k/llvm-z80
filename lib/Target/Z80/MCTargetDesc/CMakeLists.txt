add_llvm_library(LLVMZ80Desc
    Z80MCTargetDesc.cpp
)

add_dependencies(LLVMZ80Desc LLVMCommonTableGen)
