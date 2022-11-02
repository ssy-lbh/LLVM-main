#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_ZYNQ16_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_ZYNQ16_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {

class ZYNQ16TargetInfo : public TargetInfo {
protected:
  std::string ABI, CPU;
  static const Builtin::Info BuiltinInfo[];

public:
  ZYNQ16TargetInfo(const llvm::Triple &Triple, const TargetOptions &)
      : TargetInfo(Triple) {
    SuitableAlign = 16;
    WCharType = SignedShort;
    WIntType = UnsignedShort;
    MCountName = "_mcount";
    HasLongDouble = false;
    LongWidth = 16;
    resetDataLayout("e-m:e-p:16:16-i8:8:8-i16:16:16");
  }

  bool setCPU(const std::string &Name) override {
    if (!isValidCPUName(Name))
      return false;
    CPU = Name;
    return true;
  }

  bool setABI(const std::string &Name) override {
    if (Name == "ilp16") {
      ABI = Name;
      return true;
    }
    return false;
  }

  StringRef getABI() const override { return ABI; }
  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  ArrayRef<Builtin::Info> getTargetBuiltins() const override;

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }

  const char *getClobbers() const override { return ""; }

  StringRef getConstraintRegister(StringRef Constraint,
                                  StringRef Expression) const override {
    return Expression;
  }

  ArrayRef<const char *> getGCCRegNames() const override;

  int getEHDataRegisterNumber(unsigned RegNo) const override {
    if (RegNo == 0)
      return 2;
    else if (RegNo == 1)
      return 3;
    else
      return -1;
  }

  ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override;

  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &Info) const override;

  std::string convertConstraint(const char *&Constraint) const override;

  bool
  initFeatureMap(llvm::StringMap<bool> &Features, DiagnosticsEngine &Diags,
                 StringRef CPU,
                 const std::vector<std::string> &FeaturesVec) const override;

  bool hasFeature(StringRef Feature) const override;

  bool handleTargetFeatures(std::vector<std::string> &Features,
                            DiagnosticsEngine &Diags) override;

  bool hasBitIntType() const override { return true; }

  bool useFP16ConversionIntrinsics() const override {
    return false;
  }

  bool isValidCPUName(StringRef Name) const override;
  void fillValidCPUList(SmallVectorImpl<StringRef> &Values) const override;
  bool isValidTuneCPUName(StringRef Name) const override;
  void fillValidTuneCPUList(SmallVectorImpl<StringRef> &Values) const override;

  void setMaxAtomicWidth() override {
    MaxAtomicPromoteWidth = 16;
    MaxAtomicInlineWidth = 16;
  }
};
} // namespace targets
} // namespace clang

#endif