; RUN: llc < %s -march=z80 | FileCheck %s

define void @no_fp()
{
; CHECK: no_fp:
; CHECK-NOT: ld sp, {{[ix|iy]}}
; CHECK: ret
	ret void
}

define i8 @callee_saved(i8 %a, i8 %b, i8 %c)
{
; CHECK: callee_saved:
; CHECK: push [[REG:[a-z]+]]
; CHECK-NOT: ld sp, {{[ix|iy]}}
; CHECK: pop [[REG]]
	%v1 = add i8 %b, %c
	%v2 = add i8 %a, %v1
	ret i8 %v2
}

define i16 @formal_arg(i16 %a, i16 %b)
{
; CHECK: formal_arg:
; CHECK: push [[FP:[ix|iy]]]
; CHECK: ld sp, [[FP]]
	ret i16 %b
}

define void @alloc()
{
; CHECK: alloc:
; CHECK: push [[FP:[ix|iy]]]
; CHECK: ld sp, [[FP]]
	%ptr = alloca i8
	ret void
}

define void @call_nofp()
{
; CHECK: call_nofp:
; CHECK-NOT: ld sp, {{[ix|iy]}}
	call void @nofp()
	ret void
}

define void @call_fp()
{
; CHECK: call_fp:
; CHECK: push [[FP:[ix|iy]]]
; CHECK: ld sp, [[FP]]
	call void @fp(i16 0, i16 0)
	ret void
}

declare void @nofp()
declare void @fp(i16, i16)
