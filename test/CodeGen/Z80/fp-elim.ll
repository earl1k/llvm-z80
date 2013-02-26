; RUN llc -march=z80 < %s | FileCheck %s

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

define i8 @formal_arg(i8 %a, i16 %b, i8 %c)
{
; CHECK: formal_arg:
; CHECK: ld sp, {{[ix|iy]}}
	ret i8 %c
}
