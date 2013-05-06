# RUN: llvm-mc -triple s390x-linux-gnu -show-encoding %s | FileCheck %s

#CHECK: clfi	%r0, 0                  # encoding: [0xc2,0x0f,0x00,0x00,0x00,0x00]
#CHECK: clfi	%r0, 4294967295         # encoding: [0xc2,0x0f,0xff,0xff,0xff,0xff]
#CHECK: clfi	%r15, 0                 # encoding: [0xc2,0xff,0x00,0x00,0x00,0x00]

	clfi	%r0, 0
	clfi	%r0, (1 << 32) - 1
	clfi	%r15, 0