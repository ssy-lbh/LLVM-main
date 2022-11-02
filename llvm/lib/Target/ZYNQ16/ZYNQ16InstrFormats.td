class ZYNQ16Inst<dag outs, dag ins, string opcstr, string opnstr,
             list<dag> pattern = []>
    : Instruction {
  field bits<16> Inst;
  field bits<16> SoftFail = 0;

  let Namespace = "ZYNQ16";
  let Size = 2;
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString = opcstr # "\t" # opnstr;
  let Pattern = pattern;
}

class Fmt1RI7<bits<6> op, dag outs, dag ins, string opcstr, string opnstr,
            list<dag> pattern = []>
    : ZYNQ16Inst<outs, ins, opcstr, opnstr, pattern> {
    bits<7> imm7;
    bits<3> rd;

    let Inst{5-0} = op;
    let Inst{8-6} = rd;
    let Inst{15-9} = imm7;
}

class Fmt2R<bits<6> op, dag outs, dag ins, string opcstr, string opnstr,
            list<dag> pattern = []>
    : ZYNQ16Inst<outs, ins, opcstr, opnstr, pattern> {
    bits<3> rs;
    bits<3> rd;

    let Inst{5-0} = op;
    let Inst{8-6} = rd;
    let Inst{11-9} = rs;
    let Inst{15-12} = 0;
}

class Fmt2RI4<bits<6> op, dag outs, dag ins, string opcstr, string opnstr,
            list<dag> pattern = []>
    : ZYNQ16Inst<outs, ins, opcstr, opnstr, pattern> {
    bits<4> imm4;
    bits<3> rs;
    bits<3> rd;

    let Inst{5-0} = op;
    let Inst{8-6} = rd;
    let Inst{11-9} = rs;
    let Inst{15-12} = imm4;
}

class Fmt2RI5_LS1<bits<6> op, dag outs, dag ins, string opcstr, string opnstr,
            list<dag> pattern = []>
    : ZYNQ16Inst<outs, ins, opcstr, opnstr, pattern> {
    bits<5> imm5;
    bits<3> rs;
    bits<3> rd;

    let Inst{5-0} = op;
    let Inst{8-6} = rd;
    let Inst{11-9} = rs;
    let Inst{15-12} = imm5{4-1};
}

class FmtI13<bits<3> op, dag outs, dag ins, string opcstr, string opnstr,
            list<dag> pattern = []>
    : ZYNQ16Inst<outs, ins, opcstr, opnstr, pattern> {
    bits<13> imm13;

    let Inst{2-0} = op;
    let Inst{15-3} = imm13;
}

class Fmt1RI10<bits<3> op, dag outs, dag ins, string opcstr, string opnstr,
            list<dag> pattern = []>
    : ZYNQ16Inst<outs, ins, opcstr, opnstr, pattern> {
    bits<10> imm10;
    bits<3> rd;

    let Inst{2-0} = op;
    let Inst{5-3} = imm10{2-0};
    let Inst{8-6} = rd;
    let Inst{15-9} = imm10{9-3};
}