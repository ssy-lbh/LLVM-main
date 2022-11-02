add_llvm_component_library(LLVMZYNQ16Info
  ZYNQ16TargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  ZYNQ16
)