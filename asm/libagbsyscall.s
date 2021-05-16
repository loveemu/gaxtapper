	.include "macros/function.inc"
	.include "constants/gba_constants.inc"

	.syntax unified

	.text

	thumb_func_start CpuSet
CpuSet:
	svc 0xB
	bx lr
	thumb_func_end CpuSet

	.align 2, 0 @ Don't pad with nop.
