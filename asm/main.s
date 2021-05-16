	.include "macros/function.inc"
	.include "constants/gba_constants.inc"

	.syntax unified

	.set CPU_SET_SRC_FIXED, 0x01000000
	.set CPU_SET_16BIT,     0x00000000
	.set CPU_SET_32BIT,     0x04000000

	.set INTR_MAIN_BUFFER_SIZE, 0xA4
	.set GAX_WORK_RAM_SIZE, 0x1000

	.text

	.thumb

	thumb_func_start AgbMain
AgbMain:
	push {r4-r7,lr}

	ldr r7, =REG_IE
	ldr r1, =0x4014
	strh r1, [r7, #OFFSET_REG_WAITCNT - OFFSET_REG_IE]

	adr r3, GaxtapperSignature
	movs r0, 0x80
	lsls r4, r0, 17
AgbMain_ReadSignature:
	ldr r0, [r3]
	cmp r0, r4
	ble AgbMain_Init
	adds r3, r3, 4
	b AgbMain_ReadSignature
	.align 2, 0
GaxtapperSignature:
	.asciz "Gaxtapper 0.01 \xa9 loveemu"
	.align 2, 0
	.size GaxtapperSignature, .-GaxtapperSignature
	.pool

AgbMain_Init:
	bl InitIntrHandlers

AgbMain_Loop:
	svc 2
	b AgbMain_Loop
	.pool
	thumb_func_end AgbMain

	thumb_func_start InitIntrHandlers
InitIntrHandlers:
	push {r4,lr}

	ldr r4,=IntrMainBuffer
	ldr r0,=IntrMain
	movs r1, r4
	ldr r2,=INTR_MAIN_BUFFER_SIZE / 4 | CPU_SET_32BIT
	bl CpuSet

	ldr r0, =INTR_VECTOR
	str r4, [r0]

	ldr r2, =REG_BASE
	ldr r3, =REG_IE
	movs r1, #1
	strh r1, [r3, #OFFSET_REG_IME - OFFSET_REG_IE]
	strh r1, [r3, #OFFSET_REG_IE - OFFSET_REG_IE]
	movs r0, #8 @ DISPSTAT_VBLANK_INTR
	strh r0, [r2, #OFFSET_REG_DISPSTAT]
	pop {r4}
	pop {r0}
	bx r0
	.pool
	thumb_func_end InitIntrHandlers

	thumb_func_start IntrDummy
IntrDummy:
	bx lr
	.pool
	thumb_func_end IntrDummy

	thumb_func_start VBlankIntr
VBlankIntr:
	bx lr
	.pool
	thumb_func_end VBlankIntr

	.align 2, 0
gax2_new:
	.4byte IntrDummy @ overwritten by gaxtapper
gax2_init:
	.4byte IntrDummy @ overwritten by gaxtapper
gax_irq:
	.4byte IntrDummy @ overwritten by gaxtapper
gax_play:
	.4byte IntrDummy @ overwritten by gaxtapper

	.bss
IntrMainBuffer:
	.space INTR_MAIN_BUFFER_SIZE
	.size IntrMainBuffer, .-IntrMainBuffer

GaxWorkRam:
	.space GAX_WORK_RAM_SIZE
	.size GaxWorkRam, .-GaxWorkRam
