#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/ScopeExit.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MIRParser/MIRParser.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/InitializePasses.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LLVMRemarkStreamer.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TimeProfiler.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Utils/Cloning.h"

using namespace llvm;

static codegen::RegisterCodeGenFlags CGF;

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input bitcode>"), cl::init("-"));

static cl::opt<std::string>
InputLanguage("x", cl::desc("Input language ('ir' or 'mir')"));

static cl::opt<std::string>
OutputFilename("o", cl::desc("Output filename"), cl::value_desc("filename"));

static cl::opt<char>
    OptLevel("O",
             cl::desc("Optimization level. [-O0, -O1, -O2, or -O3] "
                      "(default = '-O2')"),
             cl::Prefix, cl::init(' '));

static cl::opt<bool> TimeTrace("time-trace", cl::desc("Record time trace"));

static cl::opt<unsigned> TimeTraceGranularity(
    "time-trace-granularity",
    cl::desc(
        "Minimum time granularity (in microseconds) traced by time profiler"),
    cl::init(500), cl::Hidden);

static cl::opt<std::string>
    TimeTraceFile("time-trace-file",
                  cl::desc("Specify time trace file destination"),
                  cl::value_desc("filename"));

static int compileModule(LLVMContext&);

[[noreturn]] static void reportError(Twine Msg, StringRef Filename = "") {
    SmallString<256> Prefix;
    if (!Filename.empty()) {
        if (Filename == "-")
        Filename = "<stdin>";
        ("'" + Twine(Filename) + "': ").toStringRef(Prefix);
    }
    WithColor::error(errs(), "llc-fpga") << Prefix << Msg << "\n";
    exit(1);
}

[[noreturn]] static void reportError(Error Err, StringRef Filename) {
    assert(Err);
    handleAllErrors(createFileError(Filename, std::move(Err)),
                    [&](const ErrorInfoBase &EI) { reportError(EI.message()); });
    llvm_unreachable("reportError() should not return");
}

static std::unique_ptr<ToolOutputFile> GetOutputStream() {
    // If we don't yet have an output filename, make one.
    if (OutputFilename.empty()) {
        if (InputFilename == "-")
            OutputFilename = "-";
        else {
            // If InputFilename ends in .bc or .ll, remove it.
            StringRef IFN = InputFilename;
            if (IFN.endswith(".bc") || IFN.endswith(".ll"))
                OutputFilename = std::string(IFN.drop_back(3));
            else if (IFN.endswith(".mir"))
                OutputFilename = std::string(IFN.drop_back(4));
            else
                OutputFilename = std::string(IFN);

            OutputFilename += ".bin";
        }
    }

    // Decide if we need "binary" output.
    bool Binary = false;
    switch (codegen::getFileType()) {
    case CGFT_AssemblyFile:
        break;
    case CGFT_ObjectFile:
    case CGFT_Null:
        Binary = true;
        break;
    }

    // Open the file.
    std::error_code EC;
    sys::fs::OpenFlags OpenFlags = sys::fs::OF_None;
    if (!Binary)
        OpenFlags |= sys::fs::OF_TextWithCRLF;
    auto FDOut = std::make_unique<ToolOutputFile>(OutputFilename, EC, OpenFlags);
    if (EC) {
        reportError(EC.message());
        return nullptr;
    }

    return FDOut;
}

int main(int argc, char **argv){
    InitLLVM X(argc, argv);

    EnableDebugBuffering = true;

    // 推测用于初始化全局单例Pass并注册
    // 后续可PassRegistry::getPassRegistry()->getPassInfo()->get...() 获取已初始化完毕的Pass
    // 再使用PassManager运行Pass

    // PassRegistry* Registry = PassRegistry::getPassRegistry();
    // initializeCore(*Registry);
    // initializeCodeGen(*Registry);
    // initializeScalarOpts(*Registry);
    // initializeVectorization(*Registry);

    // Register the target printer for --version.
    cl::AddExtraVersionPrinter(TargetRegistry::printRegisteredTargetsForVersion);

    cl::ParseCommandLineOptions(argc, argv, "llvm system compiler\n");

    if (TimeTrace)
        timeTraceProfilerInitialize(TimeTraceGranularity, argv[0]);
    auto TimeTraceScopeExit = make_scope_exit([]() {
        if (TimeTrace) {
            if (auto E = timeTraceProfilerWrite(TimeTraceFile, OutputFilename)) {
                handleAllErrors(std::move(E), [&](const StringError &SE) {
                errs() << SE.getMessage() << "\n";
                });
                return;
            }
            timeTraceProfilerCleanup();
        }
    });

    if (InputLanguage != "" && InputLanguage != "ir" && InputLanguage != "mir")
        reportError("input language must be '', 'IR' or 'MIR'");

    LLVMContext Context;
    compileModule(Context);

    return 0;
}

static int compileModule(LLVMContext &Context){
    SMDiagnostic Err;
    std::unique_ptr<Module> M;

    // 模块
    M = parseIRFile(InputFilename, Err, Context);
    DataLayout DL(M.get());

    std::unique_ptr<ToolOutputFile> Out = GetOutputStream();
    auto OS = &Out->os();

    // 层次结构 模块=>函数=>基本块=>指令

    struct Ins {
        unsigned short Opcode : 3;
        unsigned short Rd : 3;
        unsigned short Rs : 3;
        unsigned short Funct : 3;
        unsigned short Imm : 4;
    };

    Function *F = M->getFunction("main");
    // 函数
    for (BasicBlock *BB : depth_first(F)){
        // 基本块
        for (auto& I : *BB){
            // 指令
            // r1 => SP
            // r2 => BP
            // mov r0, 1
            // sll r0, 10[0x400第二个Block Memory]
            // mov r1, r0
            // mov r2, r0

            // push BP
            // mov BP, SP
            // sub SP, ... <= alloca

            // ...

            // mov SP, BP
            // pop BP
            // ret

            if (auto LI = dyn_cast<LoadInst>(&I)){
                llvm::dbgs() << "Load "
                        << *LI->getType()
                        << " "
                        << *LI->getPointerOperand()
                        << "\n";
                continue;
            }
            if (auto SI = dyn_cast<StoreInst>(&I)){
                llvm::dbgs() << "Store "
                        << *SI->getType()
                        << " "
                        << *SI->getPointerOperand()
                        << "\n";
                continue;
            }
            if (auto AI = dyn_cast<AllocaInst>(&I)){
                llvm::dbgs() << "Alloca "
                        << *AI->getType()
                        << "\n";
                continue;
            }
            if (auto BO = dyn_cast<BinaryOperator>(&I)){
                llvm::dbgs() << BO->getOpcodeName()
                        << " ";
                switch (BO->getOpcode()){
                case Instruction::Add:
                    BO->getOperand(0)->printAsOperand(llvm::dbgs());
                    llvm::dbgs() << ", ";
                    BO->getOperand(1)->printAsOperand(llvm::dbgs());
                    break;
                case Instruction::Sub:
                    BO->getOperand(0)->printAsOperand(llvm::dbgs());
                    llvm::dbgs() << ", ";
                    BO->getOperand(1)->printAsOperand(llvm::dbgs());
                    break;
                case Instruction::Shl:
                    BO->getOperand(0)->printAsOperand(llvm::dbgs());
                    llvm::dbgs() << ", ";
                    BO->getOperand(1)->printAsOperand(llvm::dbgs());
                    break;
                case Instruction::LShr:
                    BO->getOperand(0)->printAsOperand(llvm::dbgs());
                    llvm::dbgs() << ", ";
                    BO->getOperand(1)->printAsOperand(llvm::dbgs());
                    break;
                case Instruction::AShr:
                    BO->getOperand(0)->printAsOperand(llvm::dbgs());
                    llvm::dbgs() << ", ";
                    BO->getOperand(1)->printAsOperand(llvm::dbgs());
                    break;
                default:
                    break;
                }
                llvm::dbgs() << "\n";
                continue;
            }
            llvm::dbgs() << I.getOpcodeName() << "\n";
        }
    }

    return 0;
}