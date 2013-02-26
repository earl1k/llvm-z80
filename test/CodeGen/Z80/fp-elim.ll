; RUN: llc < %s -march=z80 | FileCheck %s

define void @no_fp()
{
; CHECK: no_fp:
; CHECK-NOT: ld sp, {{[ix|iy]}}
; CHECK: ret
	ret void
}

define i8 @add3(i8 %a, i8 %b, i8 %c)
{
; CHECK: add3:
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
; CHECK: ld sp, {{[ix|iy]}}
	ret i16 %b
}

define void @alloc()
{
; CHECK: alloc:
; CHECK: ld sp, {{[ix|iy]}}
	%ptr = alloca i8
	ret void
}
