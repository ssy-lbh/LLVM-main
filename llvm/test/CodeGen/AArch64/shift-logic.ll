; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=aarch64-- | FileCheck %s

define i8 @shl_and(i8 %x, i8 %y) nounwind {
; CHECK-LABEL: shl_and:
; CHECK:       // %bb.0:
; CHECK-NEXT:    lsl w8, w0, #5
; CHECK-NEXT:    and w0, w8, w1, lsl #2
; CHECK-NEXT:    ret
  %sh0 = shl i8 %x, 3
  %r = and i8 %sh0, %y
  %sh1 = shl i8 %r, 2
  ret i8 %sh1
}

define i16 @shl_or(i16 %x, i16 %y) nounwind {
; CHECK-LABEL: shl_or:
; CHECK:       // %bb.0:
; CHECK-NEXT:    lsl w8, w0, #12
; CHECK-NEXT:    orr w0, w8, w1, lsl #7
; CHECK-NEXT:    ret
  %sh0 = shl i16 %x, 5
  %r = or i16 %y, %sh0
  %sh1 = shl i16 %r, 7
  ret i16 %sh1
}

define i32 @shl_xor(i32 %x, i32 %y) nounwind {
; CHECK-LABEL: shl_xor:
; CHECK:       // %bb.0:
; CHECK-NEXT:    lsl w8, w0, #12
; CHECK-NEXT:    eor w0, w8, w1, lsl #7
; CHECK-NEXT:    ret
  %sh0 = shl i32 %x, 5
  %r = xor i32 %sh0, %y
  %sh1 = shl i32 %r, 7
  ret i32 %sh1
}

define i64 @lshr_and(i64 %x, i64 %y) nounwind {
; CHECK-LABEL: lshr_and:
; CHECK:       // %bb.0:
; CHECK-NEXT:    lsr x8, x0, #12
; CHECK-NEXT:    and x0, x8, x1, lsr #7
; CHECK-NEXT:    ret
  %sh0 = lshr i64 %x, 5
  %r = and i64 %y, %sh0
  %sh1 = lshr i64 %r, 7
  ret i64 %sh1
}

define <4 x i32> @lshr_or(<4 x i32> %x, <4 x i32> %y) nounwind {
; CHECK-LABEL: lshr_or:
; CHECK:       // %bb.0:
; CHECK-NEXT:    ushr v1.4s, v1.4s, #7
; CHECK-NEXT:    ushr v0.4s, v0.4s, #12
; CHECK-NEXT:    orr v0.16b, v0.16b, v1.16b
; CHECK-NEXT:    ret
  %sh0 = lshr <4 x i32> %x, <i32 5, i32 5, i32 5, i32 5>
  %r = or <4 x i32> %sh0, %y
  %sh1 = lshr <4 x i32> %r, <i32 7, i32 7, i32 7, i32 7>
  ret <4 x i32> %sh1
}

define <8 x i16> @lshr_xor(<8 x i16> %x, <8 x i16> %y) nounwind {
; CHECK-LABEL: lshr_xor:
; CHECK:       // %bb.0:
; CHECK-NEXT:    ushr v1.8h, v1.8h, #7
; CHECK-NEXT:    ushr v0.8h, v0.8h, #12
; CHECK-NEXT:    eor v0.16b, v0.16b, v1.16b
; CHECK-NEXT:    ret
  %sh0 = lshr <8 x i16> %x, <i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5, i16 5>
  %r = xor <8 x i16> %y, %sh0
  %sh1 = lshr <8 x i16> %r, <i16 7, i16 7, i16 7, i16 7, i16 7, i16 7, i16 7, i16 7>
  ret <8 x i16> %sh1
}


define <16 x i8> @ashr_and(<16 x i8> %x, <16 x i8> %y) nounwind {
; CHECK-LABEL: ashr_and:
; CHECK:       // %bb.0:
; CHECK-NEXT:    sshr v1.16b, v1.16b, #2
; CHECK-NEXT:    sshr v0.16b, v0.16b, #5
; CHECK-NEXT:    and v0.16b, v0.16b, v1.16b
; CHECK-NEXT:    ret
  %sh0 = ashr <16 x i8> %x, <i8 3, i8 3, i8 3, i8 3, i8 3, i8 3, i8 3, i8 3, i8 3, i8 3, i8 3, i8 3, i8 3, i8 3, i8 3, i8 3>
  %r = and <16 x i8> %y, %sh0
  %sh1 = ashr <16 x i8> %r, <i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2>
  ret <16 x i8> %sh1
}

define <2 x i64> @ashr_or(<2 x i64> %x, <2 x i64> %y) nounwind {
; CHECK-LABEL: ashr_or:
; CHECK:       // %bb.0:
; CHECK-NEXT:    sshr v1.2d, v1.2d, #7
; CHECK-NEXT:    sshr v0.2d, v0.2d, #12
; CHECK-NEXT:    orr v0.16b, v0.16b, v1.16b
; CHECK-NEXT:    ret
  %sh0 = ashr <2 x i64> %x, <i64 5, i64 5>
  %r = or <2 x i64> %sh0, %y
  %sh1 = ashr <2 x i64> %r, <i64 7, i64 7>
  ret <2 x i64> %sh1
}

define i32 @ashr_xor(i32 %x, i32 %y) nounwind {
; CHECK-LABEL: ashr_xor:
; CHECK:       // %bb.0:
; CHECK-NEXT:    asr w8, w0, #12
; CHECK-NEXT:    eor w0, w8, w1, asr #7
; CHECK-NEXT:    ret
  %sh0 = ashr i32 %x, 5
  %r = xor i32 %y, %sh0
  %sh1 = ashr i32 %r, 7
  ret i32 %sh1
}

define i32 @shr_mismatch_xor(i32 %x, i32 %y) nounwind {
; CHECK-LABEL: shr_mismatch_xor:
; CHECK:       // %bb.0:
; CHECK-NEXT:    eor w8, w1, w0, asr #5
; CHECK-NEXT:    lsr w0, w8, #7
; CHECK-NEXT:    ret
  %sh0 = ashr i32 %x, 5
  %r = xor i32 %y, %sh0
  %sh1 = lshr i32 %r, 7
  ret i32 %sh1
}

define i32 @ashr_overshift_xor(i32 %x, i32 %y) nounwind {
; CHECK-LABEL: ashr_overshift_xor:
; CHECK:       // %bb.0:
; CHECK-NEXT:    eor w8, w1, w0, asr #15
; CHECK-NEXT:    asr w0, w8, #17
; CHECK-NEXT:    ret
  %sh0 = ashr i32 %x, 15
  %r = xor i32 %y, %sh0
  %sh1 = ashr i32 %r, 17
  ret i32 %sh1
}

define i32 @lshr_or_extra_use(i32 %x, i32 %y, i32* %p) nounwind {
; CHECK-LABEL: lshr_or_extra_use:
; CHECK:       // %bb.0:
; CHECK-NEXT:    orr w8, w1, w0, lsr #5
; CHECK-NEXT:    lsr w0, w8, #7
; CHECK-NEXT:    str w8, [x2]
; CHECK-NEXT:    ret
  %sh0 = lshr i32 %x, 5
  %r = or i32 %sh0, %y
  store i32 %r, i32* %p
  %sh1 = lshr i32 %r, 7
  ret i32 %sh1
}