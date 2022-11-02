let Namespace = "ZYNQ16" in {
class ZYNQ16Reg<bits<16> Enc, string n, list<string> alt = []>
    : Register<n> {
    let HWEncoding = Enc;
    let AltNames = alt;
}

let FallbackRegAltNameIndex = NoRegAltName in
def RegAliasName : RegAltNameIndex;
}

let RegAltNameIndices = [RegAliasName] in {
    def R0  : ZYNQ16Reg<0,  "r0", ["b0"]>, DwarfRegNum<[0]>;
    def R1  : ZYNQ16Reg<1,  "r1", ["b1"]>, DwarfRegNum<[1]>;
    def R2  : ZYNQ16Reg<2,  "r2", ["bp"]>, DwarfRegNum<[2]>;
    def R3  : ZYNQ16Reg<3,  "r3", ["sp"]>, DwarfRegNum<[3]>;
    def R4  : ZYNQ16Reg<4,  "r4", ["a0"]>, DwarfRegNum<[4]>;
    def R5  : ZYNQ16Reg<5,  "r5", ["a1"]>, DwarfRegNum<[5]>;
    def R6  : ZYNQ16Reg<6,  "r6", ["a2"]>, DwarfRegNum<[6]>;
    def R7  : ZYNQ16Reg<7,  "r7", ["a3"]>, DwarfRegNum<[7]>;
}

def GRLenVT : ValueTypeByHwMode<[ZYNQ16],
                                [i16]>;
def GRLenRI : RegInfoByHwMode<
      [ZYNQ16],
      [RegInfo<16,16,16>]>;

// The order of registers represents the preferred allocation sequence.
// Registers are listed in the order caller-save, callee-save, specials.
def GPR : RegisterClass<"ZYNQ16", [GRLenVT], 16, (add
    // Argument registers (a0...a7)
    (sequence "R%u", 0, 1),
    // Temporary registers (t0...t8)
    (sequence "R%u", 4, 7),
    // Specials (r0, ra, tp, sp)
    (sequence "R%u", 2, 3)
  )> {
  let RegInfos = GRLenRI;
}