#include "llvm/Transforms/Utils/MyHello.h"
#include "llvm/InitializePasses.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils.h"

using namespace llvm;

MyHelloPass::MyHelloPass() = default;

PreservedAnalyses MyHelloPass::run(Function &F, FunctionAnalysisManager &AM){
    dbgs() << "my hello new pass on: ";
    dbgs().write_escaped(F.getName()) << '\n';
    return PreservedAnalyses::all();
}
