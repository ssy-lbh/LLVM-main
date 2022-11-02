#include "ZYNQ16TargetMachine.h"
#include "ZYNQ16.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "zynq16"

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeZYNQ16Target() {

}