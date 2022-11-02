include "llvm/Target/Target.td"

// features
def Is16Bit : SubtargetFeature<"16bit-mode", "Is16Bit", "true",
                               "16-bit mode">;

def FeatureSoftMult : SubtargetFeature<"soft-mult", "UseSoftMult", "true",
                                       "Use software multiplication">;

def FeatureSoftDiv : SubtargetFeature<"soft-div", "UseSoftDiv", "true",
                                      "Use software division">;

defvar ZYNQ16 = DefaultMode;

// registers, calling conversions, instructions
include "ZYNQ16RegisterInfo.td"
//include "ZYNQ16CallingConv.td"
include "ZYNQ16InstrInfo.td"

// processor
def : ProcessorModel<"generic-zynq16", NoSchedModel, [Is16Bit]>;

// target
def ZYNQ16InstrInfo : InstrInfo {
  let guessInstructionProperties = 1;
}

def ZYNQ16AsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = 1;
  let AllowDuplicateRegisterNames = 1;
}

def ZYNQ16AsmParserVariant : AsmParserVariant {
  int Variant = 0;
  string RegisterPrefix = "$";
}

def ZYNQ16AsmWriter : AsmWriter {
  int PassSubtarget = 1;
}

def ZYNQ16 : Target {
  let InstructionSet = ZYNQ16InstrInfo;
  let AssemblyParsers = [ZYNQ16AsmParser];
  let AssemblyParserVariants = [ZYNQ16AsmParserVariant];
  let AssemblyWriters = [ZYNQ16AsmWriter];
  let AllowRegisterRenaming = 1;
}