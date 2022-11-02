#ifndef LLVM_LIB_TARGET_ZYNQ16_ZYNQ16TARGETMACHINE_H
#define LLVM_LIB_TARGET_ZYNQ16_ZYNQ16TARGETMACHINE_H

#include "ZYNQ16Subtarget.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Target/TargetMachine.h"
#include <memory>

namespace llvm {

class StringRef;
class TargetTransformInfo;

class ZYNQ16TargetMachine final : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  mutable StringMap<std::unique_ptr<ZYNQ16Subtarget>> SubtargetMap;
  // True if this is used in JIT.
  bool IsJIT;

public:
  ZYNQ16TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options,
                   Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                   CodeGenOpt::Level OL, bool JIT);
  ~ZYNQ16TargetMachine() override;

  const ZYNQ16Subtarget *getSubtargetImpl(const Function &F) const override;
  // DO NOT IMPLEMENT: There is no such thing as a valid default subtarget,
  // subtargets are per-function entities based on the target-specific
  // attributes of each function.
  const ZYNQ16Subtarget *getSubtargetImpl() const = delete;

  TargetTransformInfo getTargetTransformInfo(const Function &F) const override;

  // Set up the pass pipeline.
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  bool isJIT() const { return IsJIT; }

  bool isNoopAddrSpaceCast(unsigned SrcAS, unsigned DestAS) const override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_ZYNQ16_ZYNQ16TARGETMACHINE_H
