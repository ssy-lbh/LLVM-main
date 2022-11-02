add_llvm_component_group(ZYNQ16)

set(LLVM_TARGET_DEFINITIONS ZYNQ16.td)

tablegen(LLVM ZYNQ16GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM ZYNQ16GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM ZYNQ16GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM ZYNQ16GenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM ZYNQ16GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM ZYNQ16GenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM ZYNQ16GenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM ZYNQ16GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM ZYNQ16GenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(ZYNQ16CommonTableGen)

file(GLOB sources *.cpp)

add_llvm_target(ZYNQ16CodeGen ${sources}
  LINK_COMPONENTS
  Analysis
  AsmPrinter
  CodeGen
  Core
  Instrumentation
  MC
  SelectionDAG
  Support
  Target
  TransformUtils
  GlobalISel
  ProfileData
  CFGuard

  ADD_TO_COMPONENT
  ZYNQ16
)

add_subdirectory(TargetInfo)
