//===-- ZYNQ16Subtarget.h - Define Subtarget for the ZYNQ16 ----------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the ZYNQ16 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZYNQ16_ZYNQ16SUBTARGET_H
#define LLVM_LIB_TARGET_ZYNQ16_ZYNQ16SUBTARGET_H

#include "ZYNQ16FrameLowering.h"
#include "ZYNQ16ISelLowering.h"
#include "ZYNQ16InstrInfo.h"
#include "ZYNQ16SelectionDAGInfo.h"
#include "llvm/ADT/Triple.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/CallingConv.h"
#include <climits>
#include <memory>

#define GET_SUBTARGETINFO_HEADER
#include "ZYNQ16GenSubtargetInfo.inc"

namespace llvm {

class CallLowering;
class GlobalValue;
class InstructionSelector;
class LegalizerInfo;
class RegisterBankInfo;
class StringRef;
class TargetMachine;

/// The ZYNQ16 backend supports a number of different styles of PIC.
///
namespace PICStyles {

enum class Style {
  StubPIC,          // Used on i386-darwin in pic mode.
  GOT,              // Used on 32 bit elf on when in pic mode.
  RIPRel,           // Used on ZYNQ16-64 when in pic mode.
  None              // Set when not in pic mode.
};

} // end namespace PICStyles

class ZYNQ16Subtarget final : public ZYNQ16GenSubtargetInfo {
  /// Which PIC style to use
  PICStyles::Style PICStyle;

  const TargetMachine &TM;
  
  /// The minimum alignment known to hold of the stack frame on
  /// entry to the function and which must be maintained by every function.
  Align stackAlignment = Align(4);

  Align TileConfigAlignment = Align(4);

  /// Max. memset / memcpy size that is turned into rep/movs, rep/stos ops.
  ///
  // FIXME: this is a known good value for Yonah. How about others?
  unsigned MaxInlineSizeThreshold = 128;

  /// What processor and OS we're targeting.
  Triple TargetTriple;

  /// GlobalISel related APIs.
  std::unique_ptr<CallLowering> CallLoweringInfo;
  std::unique_ptr<LegalizerInfo> Legalizer;
  std::unique_ptr<RegisterBankInfo> RegBankInfo;
  std::unique_ptr<InstructionSelector> InstSelector;

  /// Override the stack alignment.
  MaybeAlign StackAlignOverride;

  /// Preferred vector width from function attribute.
  unsigned PreferVectorWidthOverride;

  /// Resolved preferred vector width from function attribute and subtarget
  /// features.
  unsigned PreferVectorWidth = UINT32_MAX;

  /// Required vector width from function attribute.
  unsigned RequiredVectorWidth;

  ZYNQ16SelectionDAGInfo TSInfo;
  // Ordering here is important. ZYNQ16InstrInfo initializes ZYNQ16RegisterInfo which
  // ZYNQ16TargetLowering needs.
  ZYNQ16InstrInfo InstrInfo;
  ZYNQ16TargetLowering TLInfo;
  ZYNQ16FrameLowering FrameLowering;

public:
  /// This constructor initializes the data members to match that
  /// of the specified triple.
  ///
  ZYNQ16Subtarget(const Triple &TT, StringRef CPU, StringRef TuneCPU, StringRef FS,
               const ZYNQ16TargetMachine &TM, MaybeAlign StackAlignOverride,
               unsigned PreferVectorWidthOverride,
               unsigned RequiredVectorWidth);

  const ZYNQ16TargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }

  const ZYNQ16InstrInfo *getInstrInfo() const override { return &InstrInfo; }

  const ZYNQ16FrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }

  const ZYNQ16SelectionDAGInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }

  const ZYNQ16RegisterInfo *getRegisterInfo() const override {
    return &getInstrInfo()->getRegisterInfo();
  }

  unsigned getTileConfigSize() const { return 64; }
  Align getTileConfigAlignment() const { return TileConfigAlignment; }

  /// Returns the minimum alignment known to hold of the
  /// stack frame on entry to the function and which must be maintained by every
  /// function for this subtarget.
  Align getStackAlignment() const { return stackAlignment; }

  /// Returns the maximum memset / memcpy size
  /// that still makes it profitable to inline the call.
  unsigned getMaxInlineSizeThreshold() const { return MaxInlineSizeThreshold; }

  /// ParseSubtargetFeatures - Parses features string setting specified
  /// subtarget options.  Definition of function is auto generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);

  /// Methods used by Global ISel
  const CallLowering *getCallLowering() const override;
  InstructionSelector *getInstructionSelector() const override;
  const LegalizerInfo *getLegalizerInfo() const override;
  const RegisterBankInfo *getRegBankInfo() const override;

private:
  /// Initialize the full set of dependencies so we can use an initializer
  /// list for ZYNQ16Subtarget.
  ZYNQ16Subtarget &initializeSubtargetDependencies(StringRef CPU,
                                                StringRef TuneCPU,
                                                StringRef FS);
  void initSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);

public:

#define GET_SUBTARGETINFO_MACRO(ATTRIBUTE, DEFAULT, GETTER)                    \
  bool GETTER() const { return ATTRIBUTE; }
#include "ZYNQ16GenSubtargetInfo.inc"

  /// Is this ZYNQ16_64 with the ILP32 programming model (x32 ABI)?
  bool isTarget64BitILP32() const {
    return Is64Bit && (TargetTriple.isX32() || TargetTriple.isOSNaCl());
  }

  /// Is this ZYNQ16_64 with the LP64 programming model (standard AMD64, no x32)?
  bool isTarget64BitLP64() const {
    return Is64Bit && (!TargetTriple.isX32() && !TargetTriple.isOSNaCl());
  }

  PICStyles::Style getPICStyle() const { return PICStyle; }
  void setPICStyle(PICStyles::Style Style)  { PICStyle = Style; }

  bool canUseLAHFSAHF() const { return hasLAHFSAHF64() || !is64Bit(); }
  // These are generic getters that OR together all of the thunk types
  // supported by the subtarget. Therefore useIndirectThunk*() will return true
  // if any respective thunk feature is enabled.
  bool useIndirectThunkCalls() const {
    return useRetpolineIndirectCalls() || useLVIControlFlowIntegrity();
  }
  bool useIndirectThunkBranches() const {
    return useRetpolineIndirectBranches() || useLVIControlFlowIntegrity();
  }

  unsigned getPreferVectorWidth() const { return PreferVectorWidth; }
  unsigned getRequiredVectorWidth() const { return RequiredVectorWidth; }

  // Helper functions to determine when we should allow widening to 512-bit
  // during codegen.
  // TODO: Currently we're always allowing widening on CPUs without VLX,
  // because for many cases we don't have a better option.
  bool canExtendTo512DQ() const {
    return hasAVX512() && (!hasVLX() || getPreferVectorWidth() >= 512);
  }
  bool canExtendTo512BW() const  {
    return hasBWI() && canExtendTo512DQ();
  }

  // If there are no 512-bit vectors and we prefer not to use 512-bit registers,
  // disable them in the legalizer.
  bool useAVX512Regs() const {
    return hasAVX512() && (canExtendTo512DQ() || RequiredVectorWidth > 256);
  }

  bool useBWIRegs() const {
    return hasBWI() && useAVX512Regs();
  }

  bool isXRaySupported() const override { return is64Bit(); }

  /// Use mfence if we have SSE2 or we're on ZYNQ16-64 (even if we asked for
  /// no-sse2). There isn't any reason to disable it if the target processor
  /// supports it.
  bool hasMFence() const { return hasSSE2() || is64Bit(); }

  const Triple &getTargetTriple() const { return TargetTriple; }

  bool isTargetDarwin() const { return TargetTriple.isOSDarwin(); }
  bool isTargetFreeBSD() const { return TargetTriple.isOSFreeBSD(); }
  bool isTargetDragonFly() const { return TargetTriple.isOSDragonFly(); }
  bool isTargetSolaris() const { return TargetTriple.isOSSolaris(); }
  bool isTargetPS() const { return TargetTriple.isPS(); }

  bool isTargetELF() const { return TargetTriple.isOSBinFormatELF(); }
  bool isTargetCOFF() const { return TargetTriple.isOSBinFormatCOFF(); }
  bool isTargetMachO() const { return TargetTriple.isOSBinFormatMachO(); }

  bool isTargetLinux() const { return TargetTriple.isOSLinux(); }
  bool isTargetKFreeBSD() const { return TargetTriple.isOSKFreeBSD(); }
  bool isTargetGlibc() const { return TargetTriple.isOSGlibc(); }
  bool isTargetAndroid() const { return TargetTriple.isAndroid(); }
  bool isTargetNaCl() const { return TargetTriple.isOSNaCl(); }
  bool isTargetNaCl32() const { return isTargetNaCl() && !is64Bit(); }
  bool isTargetNaCl64() const { return isTargetNaCl() && is64Bit(); }
  bool isTargetMCU() const { return TargetTriple.isOSIAMCU(); }
  bool isTargetFuchsia() const { return TargetTriple.isOSFuchsia(); }

  bool isTargetWindowsMSVC() const {
    return TargetTriple.isWindowsMSVCEnvironment();
  }

  bool isTargetWindowsCoreCLR() const {
    return TargetTriple.isWindowsCoreCLREnvironment();
  }

  bool isTargetWindowsCygwin() const {
    return TargetTriple.isWindowsCygwinEnvironment();
  }

  bool isTargetWindowsGNU() const {
    return TargetTriple.isWindowsGNUEnvironment();
  }

  bool isTargetWindowsItanium() const {
    return TargetTriple.isWindowsItaniumEnvironment();
  }

  bool isTargetCygMing() const { return TargetTriple.isOSCygMing(); }

  bool isOSWindows() const { return TargetTriple.isOSWindows(); }

  bool isTargetWin64() const { return Is64Bit && isOSWindows(); }

  bool isTargetWin32() const { return !Is64Bit && isOSWindows(); }

  bool isPICStyleGOT() const { return PICStyle == PICStyles::Style::GOT; }
  bool isPICStyleRIPRel() const { return PICStyle == PICStyles::Style::RIPRel; }

  bool isPICStyleStubPIC() const {
    return PICStyle == PICStyles::Style::StubPIC;
  }

  bool isPositionIndependent() const;

  bool isCallingConvWin64(CallingConv::ID CC);

  /// Classify a global variable reference for the current subtarget according
  /// to how we should reference it in a non-pcrel context.
  unsigned char classifyLocalReference(const GlobalValue *GV) const;

  unsigned char classifyGlobalReference(const GlobalValue *GV,
                                        const Module &M) const;
  unsigned char classifyGlobalReference(const GlobalValue *GV) const;

  /// Classify a global function reference for the current subtarget.
  unsigned char classifyGlobalFunctionReference(const GlobalValue *GV,
                                                const Module &M) const;
  unsigned char classifyGlobalFunctionReference(const GlobalValue *GV) const;

  /// Classify a blockaddress reference for the current subtarget according to
  /// how we should reference it in a non-pcrel context.
  unsigned char classifyBlockAddressReference() const;

  /// Return true if the subtarget allows calls to immediate address.
  bool isLegalToCallImmediateAddr() const;

  /// Return whether FrameLowering should always set the "extended frame
  /// present" bit in FP, or set it based on a symbol in the runtime.
  bool swiftAsyncContextIsDynamicallySet() const {
    // Older OS versions (particularly system unwinders) are confused by the
    // Swift extended frame, so when building code that might be run on them we
    // must dynamically query the concurrency library to determine whether
    // extended frames should be flagged as present.
    const Triple &TT = getTargetTriple();

    unsigned Major = TT.getOSVersion().getMajor();
    switch(TT.getOS()) {
    default:
      return false;
    case Triple::IOS:
    case Triple::TvOS:
      return Major < 15;
    case Triple::WatchOS:
      return Major < 8;
    case Triple::MacOSX:
    case Triple::Darwin:
      return Major < 12;
    }
  }

  /// If we are using indirect thunks, we need to expand indirectbr to avoid it
  /// lowering to an actual indirect jump.
  bool enableIndirectBrExpand() const override {
    return useIndirectThunkBranches();
  }

  /// Enable the MachineScheduler pass for all ZYNQ16 subtargets.
  bool enableMachineScheduler() const override { return true; }

  bool enableEarlyIfConversion() const override;

  void getPostRAMutations(std::vector<std::unique_ptr<ScheduleDAGMutation>>
                              &Mutations) const override;

  AntiDepBreakMode getAntiDepBreakMode() const override {
    return TargetSubtargetInfo::ANTIDEP_CRITICAL;
  }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_ZYNQ16_ZYNQ16SUBTARGET_H
