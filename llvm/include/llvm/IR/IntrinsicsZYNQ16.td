def zynq16_16_ty : LLVMType<i16>;

let TargetPrefix = "zynq16" in {
def int_zynq16_max_qb: ClangBuiltin<"__builtin_zynq16_max_qb">,
  Intrinsic<[llvm_i16_ty], [llvm_i16_ty, llvm_i16_ty], [Commutative]>;

def int_zynq16_min_qb: ClangBuiltin<"__builtin_zynq16_min_qb">,
  Intrinsic<[llvm_i16_ty], [llvm_i16_ty, llvm_i16_ty], [Commutative]>;
}