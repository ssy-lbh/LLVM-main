#include "ZYNQ16TargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheZYNQ16Target() {
    static Target TheZYNQ16Target;
    return TheZYNQ16Target;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeZYNQ16TargetInfo() {
  RegisterTarget<Triple::loongarch32, /*HasJIT=*/false> X(
      getTheZYNQ16Target(), "zynq16", "16-bit ZYNQ", "ZYNQ");
}