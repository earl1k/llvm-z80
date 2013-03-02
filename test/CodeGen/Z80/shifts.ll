; RUN: llc < %s -march=z80 | FileCheck %s

define i16 @shl16(i16 %arg, i8 %amt)
{
; CHECK: shl16:
; CHECK: cp 0
; CHECK-NEXT: jp z, 
; CHECK: sla
; CHECK-NEXT: rl
; CHECK-NEXT: dec
; CHECK-NEXT: jp nz,
	%cnt = zext i8 %amt to i16
	%res = shl i16 %arg, %cnt
	ret i16 %res
}

define i16 @lshr16(i16 %arg, i8 %amt)
{
; CHECK: lshr16:
; CHECK: cp 0
; CHECK-NEXT: jp z, 
; CHECK: srl
; CHECK-NEXT: rr
; CHECK-NEXT: dec
; CHECK-NEXT: jp nz,
	%cnt = zext i8 %amt to i16
	%res = lshr i16 %arg, %cnt
	ret i16 %res
}

define i16 @ashr16(i16 %arg, i8 %amt)
{
; CHECK: ashr16:
; CHECK: cp 0
; CHECK-NEXT: jp z, 
; CHECK: sra
; CHECK-NEXT: rr
; CHECK-NEXT: dec
; CHECK-NEXT: jp nz,
	%cnt = zext i8 %amt to i16
	%res = ashr i16 %arg, %cnt
	ret i16 %res
}
