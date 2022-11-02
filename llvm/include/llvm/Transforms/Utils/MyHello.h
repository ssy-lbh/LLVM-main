#ifndef LLVM_TRANSFORMS_UTILS_MYHELLO_H
#define LLVM_TRANSFORMS_UTILS_MYHELLO_H

#include "llvm/IR/PassManager.h"

namespace llvm {
class Function;
class FunctionPass;

class MyHelloPass : public PassInfoMixin<MyHelloPass> {
public:
    explicit MyHelloPass();
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
    static bool isRequired() { return true; }
};
}

#endif
