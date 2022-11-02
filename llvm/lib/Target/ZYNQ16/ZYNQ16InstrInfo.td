include "ZYNQ16InstrFormats.td"

class ImmAsmOperand<string prefix, int width, string suffix>
    : AsmOperandClass {
  let Name = prefix # "Imm" # width # suffix;
  let DiagnosticType = !strconcat("Invalid", Name);
  let RenderMethod = "addImmOperands";
}

class SImmAsmOperand<int width, string suffix = "">
    : ImmAsmOperand<"S", width, suffix> {
}

class UImmAsmOperand<int width, string suffix = "">
    : ImmAsmOperand<"U", width, suffix> {
}

def simm4 : Operand<GRLenVT>, ImmLeaf<GRLenVT, [{return isInt<4>(Imm);}]> {
    let ParserMatchClass = SImmAsmOperand<4>;
    let DecoderMethod = "decodeSImmOperand<4>";
}

def simm5_ls1 : Operand<GRLenVT> {
    let ParserMatchClass = SImmAsmOperand<5, "ls1">;
    let EncoderMethod = "getImmOpValueAsr1";
    let DecoderMethod = "decodeSImmOperand<5, 1>";
}

def simm7 : Operand<GRLenVT>, ImmLeaf<GRLenVT, [{return isInt<7>(Imm);}]> {
    let ParserMatchClass = SImmAsmOperand<7>;
    let DecoderMethod = "decodeSImmOperand<7>";
}

def simm10 : Operand<GRLenVT>, ImmLeaf<GRLenVT, [{return isInt<10>(Imm);}]> {
    let ParserMatchClass = SImmAsmOperand<10>;
    let DecoderMethod = "decodeSImmOperand<10>";
}

def simm13 : Operand<GRLenVT>, ImmLeaf<GRLenVT, [{return isInt<13>(Imm);}]> {
    let ParserMatchClass = SImmAsmOperand<13>;
    let DecoderMethod = "decodeSImmOperand<13>";
}

class ALU_1RI7<bits<6> op, string opstr, Operand ImmOpnd>
    : Fmt1RI7<op, (outs GPR:$rd), (ins ImmOpnd:$imm7), opstr,
            "$rd, $imm7">;

class ALU_2R<bits<6> op, string opstr>
    : Fmt2R<op, (outs GPR:$rd), (ins GPR:$rs), opstr,
            "$rd, $rs">;

class ALU_2RI4<bits<6> op, string opstr, Operand ImmOpnd>
    : Fmt2RI4<op, (outs GPR:$rd), (ins GPR:$rs, ImmOpnd:$imm4), opstr,
            "$rd, $rs, $imm4">;

class BR_I13<bits<3> op, string opstr, Operand ImmOpnd>
    : FmtI13<op, (outs), (ins ImmOpnd:$imm13), opstr,
            "$imm13"> {
    let isBranch = 1;
    let isTerminator = 1;
}

class BR_1RI10<bits<3> op, string opstr, Operand ImmOpnd>
    : Fmt1RI10<op, (outs), (ins GPR:$rd, ImmOpnd:$imm10), opstr,
            "$rd, $imm10"> {
    let isBranch = 1;
    let isTerminator = 1;
}

class BR_2RI5<bits<6> op, string opstr>
    : Fmt2RI5_LS1<op, (outs), (ins GPR:$rd, GPR:$rs, simm5_ls1:$imm5), opstr,
            "$rd, $rs, $imm5"> {
    let isBranch = 1;
    let isTerminator = 1;
}

let mayLoad = 1 in {
class LOAD_1RI7<bits<6> op, string opstr, Operand ImmOpnd>
    : Fmt1RI7<op, (outs GPR:$rd), (ins GPR:$rs, ImmOpnd:$imm7), opstr,
            "$rd, $imm7">;

class LOAD_2RI4<bits<6> op, string opstr, Operand ImmOpnd>
    : Fmt2RI4<op, (outs GPR:$rd), (ins GPR:$rs, ImmOpnd:$imm4), opstr,
            "$rd, $rs, $imm4">;
}

let mayStore = 1 in {
class STORE_1RI7<bits<6> op, string opstr, Operand ImmOpnd>
    : Fmt1RI7<op, (outs GPR:$rd), (ins GPR:$rs, ImmOpnd:$imm7), opstr,
            "$rd, $imm7">;

class STORE_2RI4<bits<6> op, string opstr, Operand ImmOpnd>
    : Fmt2RI4<op, (outs), (ins GPR:$rd, GPR:$rs, ImmOpnd:$imm4), opstr,
            "$rd, $rs, $imm4">;
}

multiclass M_ALU_IR<bits<5> op, string opstr, Operand ImmOpnd> {
    def _I : ALU_1RI7<{op, 0b0}, opstr, ImmOpnd>;
    def _R : ALU_2R<{op, 0b1}, opstr>;
}

multiclass M_LOAD_PCR<bits<5> op, string opstr> {
    def _R : LOAD_2RI4<{op, 0b0}, opstr, simm4>;
    def _PC : LOAD_1RI7<{op, 0b1}, opstr, simm7>;
}

multiclass M_STORE_PCR<bits<5> op, string opstr> {
    def _R : STORE_2RI4<{op, 0b0}, opstr, simm4>;
    def _PC : STORE_1RI7<{op, 0b1}, opstr, simm7>;
}

defm ADD : M_ALU_IR<0b00000, "add", simm7>;
defm AND : M_ALU_IR<0b00100, "and", simm7>;
defm OR : M_ALU_IR<0b01000, "or", simm7>;
defm XOR : M_ALU_IR<0b01100, "xor", simm7>;
defm SLL : M_ALU_IR<0b10000, "sll", simm7>;
defm SRL : M_ALU_IR<0b10100, "srl", simm7>;
defm SRA : M_ALU_IR<0b11000, "sra", simm7>;
defm MOV : M_ALU_IR<0b11100, "mov", simm7>;

defm LD_B : M_LOAD_PCR<0b00001, "lb">;
defm LD_W : M_LOAD_PCR<0b00101, "lw">;
defm LD_BU : M_LOAD_PCR<0b01001, "lbu">;
defm ST_B : M_STORE_PCR<0b01101, "sb">;
defm ST_W : M_STORE_PCR<0b10001, "sw">;

def LEAR : ALU_2RI4<0b101010, "lea", simm4>;
def LEA : ALU_1RI7<0b101011, "lea", simm7>;

def JMP : BR_I13<0b100, "jmp", simm13>;
def JMPR : BR_1RI10<0b101, "jmp", simm10>;

def BEQ : BR_2RI5<0b000110, "beq">;
def BNE : BR_2RI5<0b001110, "bne">;
def BLT : BR_2RI5<0b010110, "blt">;
def BGE : BR_2RI5<0b011110, "bge">;
def BLTU : BR_2RI5<0b100110, "bltu">;
def BGEU : BR_2RI5<0b101110, "bgeu">;
