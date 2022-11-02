//===--- ZYNQ16.cpp - Implement ZYNQ16 target feature support ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements ZYNQ16 TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "ZYNQ16.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/TargetParser.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::targets;

ArrayRef<const char *> ZYNQ16TargetInfo::getGCCRegNames() const {
  static const char *const GCCRegNames[] = {
      // Integer registers
      "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7"};
  return llvm::makeArrayRef(GCCRegNames);
}

ArrayRef<TargetInfo::GCCRegAlias> ZYNQ16TargetInfo::getGCCRegAliases() const {
  static const TargetInfo::GCCRegAlias GCCRegAliases[] = {
      {{"b0"}, "r0"}, {{"b1"}, "r1"}, {{"bp"}, "r2"}, {{"sp"}, "r3"},
      {{"a0"}, "r4"}, {{"a1"}, "r5"}, {{"a2"}, "r6"}, {{"a3"}, "r7"}};
  return llvm::makeArrayRef(GCCRegAliases);
}

bool ZYNQ16TargetInfo::validateAsmConstraint(
        const char *&Name, TargetInfo::ConstraintInfo &Info) const {
    switch (*Name) {
    default:
        return false;
    case 'I':
        // A 12-bit signed immediate.
        Info.setRequiresImmediate(-0x40, 0x3F);
        return true;
    case 'J':
        // Integer zero.
        Info.setRequiresImmediate(0);
        return true;
    case 'A':
        // An address that is held in a general-purpose register.
        Info.setAllowsMemory();
        return true;
    case 'S': // A symbolic address
        Info.setAllowsRegister();
        return true;
    return false;
    }
}

std::string ZYNQ16TargetInfo::convertConstraint(const char *&Constraint) const {
    return TargetInfo::convertConstraint(Constraint);
}

void ZYNQ16TargetInfo::getTargetDefines(const LangOptions &Opts,
                                       MacroBuilder &Builder) const {
  Builder.defineMacro("__ELF__");
  Builder.defineMacro("__zynq16");
}

const Builtin::Info ZYNQ16TargetInfo::BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS)                                               \
  {#ID, TYPE, ATTRS, nullptr, ALL_LANGUAGES, nullptr},
#define TARGET_BUILTIN(ID, TYPE, ATTRS, FEATURE)                               \
    {#ID, TYPE, ATTRS, nullptr, ALL_LANGUAGES, FEATURE},
#include "clang/Basic/BuiltinsZYNQ16.def"
};

ArrayRef<Builtin::Info> ZYNQ16TargetInfo::getTargetBuiltins() const {
  return llvm::makeArrayRef(BuiltinInfo, clang::ZYNQ16::LastTSBuiltin -
                                             Builtin::FirstTSBuiltin);
}

bool ZYNQ16TargetInfo::initFeatureMap(
    llvm::StringMap<bool> &Features, DiagnosticsEngine &Diags, StringRef CPU,
    const std::vector<std::string> &FeaturesVec) const {
  // ZYNQ16ISAInfo makes implications for ISA features
  std::vector<std::string> ImpliedFeatures;
  return TargetInfo::initFeatureMap(Features, Diags, CPU, ImpliedFeatures);
}

/// Return true if has this feature, need to sync with handleTargetFeatures.
bool ZYNQ16TargetInfo::hasFeature(StringRef Feature) const {
  auto Result = llvm::StringSwitch<Optional<bool>>(Feature)
                    .Case("ZYNQ16", true)
                    .Default(None);
  if (Result)
    return Result.value();

  return false;
}

/// Perform initialization based on the user configured set of features.
bool ZYNQ16TargetInfo::handleTargetFeatures(std::vector<std::string> &Features,
                                           DiagnosticsEngine &Diags) {
  if (ABI.empty())
    ABI = "ilp16";

  return true;
}

static const char* ARCH = "zynq16";

bool ZYNQ16TargetInfo::isValidCPUName(StringRef Name) const {
  return Name == ARCH;
}

void ZYNQ16TargetInfo::fillValidCPUList(
    SmallVectorImpl<StringRef> &Values) const {
  Values.push_back(ARCH);
}

bool ZYNQ16TargetInfo::isValidTuneCPUName(StringRef Name) const {
  return Name == ARCH;
}

void ZYNQ16TargetInfo::fillValidTuneCPUList(
    SmallVectorImpl<StringRef> &Values) const {
  Values.push_back(ARCH);
}
