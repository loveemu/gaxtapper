@ Gaxtapper: relocatable GSF driver block
@
@ * Using `ldr rX, =label` is not allowed
@ * Using data section such as `.bss` is not allowed

	.include "../macros/function.inc"
	.include "../macros/struct.inc"
	.include "../constants/gba_constants.inc"
	.include "../constants/gax3_constants.inc"
	.include "../constants/gax3driver_constants.inc"

	.syntax unified

	.set INTR_MAIN_BUFFER_SIZE, 0x94
	.set INTR_TABLE_LENGTH, 2

	.global Start

	.text

	.arm

Start: @ 8000000
	b Init

	.include "rom_header.inc"

	.arm
	.align 2, 0
	.global Init
Init: @ 80000C0
	mov r0, #PSR_IRQ_MODE
	msr cpsr_cf, r0
	ldr sp, sp_irq
	mov r0, #PSR_SYS_MODE
	msr cpsr_cf, r0
	ldr sp, sp_sys
	ldr r1, =INTR_VECTOR
	adr r0, IntrMain
	str r0, [r1]
	mov r0, #255 @ RESET_ALL
	svc #1 << 16
	adr r1, AgbMain+1
	mov lr, pc
	bx r1
	b . @ spin

	.align 2, 0
sp_sys: .word IWRAM_END - 0x100
sp_irq: .word IWRAM_END - 0x60
	.pool

	thumb_func_start AgbMain
AgbMain:
	push {r4-r7,lr}
	sub sp, sp, #Gax2Params_size

	ldr r0, =REG_WAITCNT
	ldr r1, =0x4014
	strh r1, [r0]

	adr r3, GaxtapperSignature
	movs r0, 0x80
	lsls r0, r0, 17 @ 0x1000000
AgbMain_ReadSignature:
	ldr r0, [r3]
	adds r3, r3, 4
	cmp r0, r4
	bgt AgbMain_ReadSignature
AgbMain_Init:
	mov r4, sp
	movs r0, r4
	bl gax2_new

	adr r5, myMinigsfParams
	ldrh r0, [r5, #o_MinigsfParams_flags]
	strh r0, [r4, #o_Gax2Params_flags]
	ldrh r0, [r5, #o_MinigsfParams_mixing_rate]
	strh r0, [r4, #o_Gax2Params_mixing_rate]
	ldrh r0, [r5, #o_MinigsfParams_volume]
	strh r0, [r4, #o_Gax2Params_volume]
	movs r0, #4
	strh r0, [r4, #o_Gax2Params_num_fx_channels]
	ldr r0, [r5, #o_MinigsfParams_fx]
	movs r3, #o_Gax2Params_fx @ PATCH: gaxtapper will change the value
	adds r3, r3, r4
	str r0, [r3]
	ldr r0, [r5, #o_MinigsfParams_music]
	adds r3, r3, #o_Gax2Params_music - o_Gax2Params_fx
	str r0, [r3]

	movs r0, r4
	bl gax2_estimate

	movs r1, #(INTR_MAIN_BUFFER_SIZE + INTR_TABLE_LENGTH * 4)
	ldr r0, DriverWorkRamStart
	adds r0, r0, r1
	str r0, [r4, #o_Gax2Params_wram]

	movs r0, r4
	bl gax2_init

	ldrh r0, [r5, #o_MinigsfParams_fxid] @ not used yet, but read to prevent optimization

	ldr r0, DriverWorkRamStart
	bl InitIntrHandlers

AgbMain_Loop:
	svc 2
	b AgbMain_Loop
	.align 2, 0
	.pool
GaxtapperSignature:
	.asciz "Ripped by Gaxtapper 0.01"
	.align 2, 0 @ Don't pad with nop.
	.size GaxtapperSignature, .-GaxtapperSignature
DriverWorkRamStart:
	.4byte 0x3000000    @ PATCH: gaxtapper will change the value
myMinigsfParams:            @ PATCH: gaxtapper will change the value
	.4byte 0x8000000    @ music
	.4byte 0x8000000    @ fx
	.2byte 0xFFFF       @ fxid
	.2byte 0xFFFF       @ flags
	.2byte 0xFFFF       @ mixing_rate
	.2byte 0xFFFF       @ volume
	.align 2, 0
	thumb_func_end AgbMain

	@ (dest: void*)
	thumb_func_start InitIntrHandlers
InitIntrHandlers:
	push {r4,lr}
	movs r4, r0 @ destination address for interrupt handler

	@ Load interrupt handler to RAM
	ldr r3, =REG_DMA3
	adr r0, IntrMain
	str r0, [r3, #OFFSET_REG_DMA3SAD - OFFSET_REG_DMA3]
	str r4, [r3, #OFFSET_REG_DMA3DAD - OFFSET_REG_DMA3]
	ldr r0, =(DMA_ENABLE | DMA_START_NOW | DMA_32BIT | DMA_SRC_INC | DMA_DEST_INC) << 16 | (INTR_MAIN_BUFFER_SIZE / 4)
	str r0, [r3, #OFFSET_REG_DMA3CNT - OFFSET_REG_DMA3]
	ldr r0, [r3, #OFFSET_REG_DMA3CNT - OFFSET_REG_DMA3]

	@ Load callback addresses as well
	movs r0, #INTR_MAIN_BUFFER_SIZE
	adds r3, r4, r0
	adr r0, VBlankIntr
	adds r0, r0, #1
	str r0, [r3]
	adr r1, IntrDummy
	adds r1, r1, #1
	str r1, [r3, 4]

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
	.align 2, 0
	.pool
	thumb_func_end InitIntrHandlers

	.align 2, 0
	arm_func_start IntrMain
IntrMain:
	mov r3, #REG_BASE
	add r3, r3, #OFFSET_REG_IE
	ldr r2, [r3]
	ldrh r1, [r3, #OFFSET_REG_IME - OFFSET_REG_IE]
	mrs r0, spsr
	stmfd sp!, {r0-r3,lr}
	mov r0, #0
	strh r0, [r3, #OFFSET_REG_IME - OFFSET_REG_IE]
	and r1, r2, r2, lsr #16
	mov r12, #0
	ands r0, r1, #INTR_FLAG_VBLANK
	bne IntrMain_FoundIntr
@	add r12, r12, 0x4
@	ands r0, r1, #INTR_FLAG_HBLANK
@	bne IntrMain_FoundIntr
@	add r12, r12, 0x4
@	ands r0, r1, #INTR_FLAG_VCOUNT
@	bne IntrMain_FoundIntr
@	add r12, r12, 0x4
@	ands r0, r1, #INTR_FLAG_TIMER0
@	bne IntrMain_FoundIntr
@	add r12, r12, 0x4
@	ands r0, r1, #INTR_FLAG_TIMER1
@	bne IntrMain_FoundIntr
@	add r12, r12, 0x4
@	ands r0, r1, #INTR_FLAG_TIMER2
@	bne IntrMain_FoundIntr
@	add r12, r12, 0x4
@	ands r0, r1, #INTR_FLAG_TIMER3
@	bne IntrMain_FoundIntr
@	add r12, r12, 0x4
@	ands r0, r1, #INTR_FLAG_SERIAL
@	bne IntrMain_FoundIntr
@	add r12, r12, 0x4
@	ands r0, r1, #INTR_FLAG_DMA0
@	bne IntrMain_FoundIntr
@	add r12, r12, 0x4
@	ands r0, r1, #INTR_FLAG_DMA1
@	bne IntrMain_FoundIntr
@	add r12, r12, 0x4
@	ands r0, r1, #INTR_FLAG_DMA2
@	bne IntrMain_FoundIntr
@	add r12, r12, 0x4
@	ands r0, r1, #INTR_FLAG_DMA3
@	bne IntrMain_FoundIntr
@	add r12, r12, 0x4
@	ands r0, r1, #INTR_FLAG_KEYPAD
@	bne IntrMain_FoundIntr
	add r12, r12, 0x4
	ands r0, r1, #INTR_FLAG_GAMEPAK
	strbne r0, [r3, #REG_SOUNDCNT_X - REG_IE]
	bne . @ spin
IntrMain_FoundIntr:
	strh r0, [r3, #OFFSET_REG_IF - OFFSET_REG_IE]
	mrs r3, cpsr
	bic r3, r3, #PSR_I_BIT | PSR_F_BIT | PSR_MODE_MASK
	orr r3, r3, #PSR_SYS_MODE
	msr cpsr_cf, r3
	adr r1, IntrTable
	add r1, r1, r12
	ldr r0, [r1]
	stmfd sp!, {lr}
	adr lr, IntrMain_RetAddr
	bx r0
IntrMain_RetAddr:
	ldmfd sp!, {lr}
	mrs r3, cpsr
	bic r3, r3, #PSR_I_BIT | PSR_F_BIT | PSR_MODE_MASK
	orr r3, r3, #PSR_I_BIT | PSR_IRQ_MODE
	msr cpsr_cf, r3
	ldmia sp!, {r0-r3,lr}
	strh r2, [r3, #OFFSET_REG_IE - OFFSET_REG_IE]
	strh r1, [r3, #OFFSET_REG_IME - OFFSET_REG_IE]
	msr spsr_cf, r0
	bx lr
	.align 2, 0
	.pool
IntrTable:
	.space INTR_TABLE_LENGTH * 4
	arm_func_end IntrMain

	thumb_func_start IntrDummy
IntrDummy:
	bx lr
	.align 2, 0
	thumb_func_end IntrDummy

	thumb_func_start VBlankIntr
VBlankIntr:
	push {lr}
	bl gax_irq
	bl gax_play
	pop {r0}
	bx r0
	.align 2, 0
	.pool
	thumb_func_end VBlankIntr

	thumb_func_start gax2_estimate
gax2_estimate:
	ldr r1, gax2_estimate_p
	bx r1
	.align 2, 0
gax2_estimate_p:
	.4byte 0x8000000 @ PATCH: gaxtapper will change the value
	thumb_func_end gax2_estimate

	thumb_func_start gax2_new
gax2_new:
	ldr r1, gax2_new_p
	bx r1
	.align 2, 0
gax2_new_p:
	.4byte 0x8000000 @ PATCH: gaxtapper will change the value
	thumb_func_end gax2_new

	thumb_func_start gax2_init
gax2_init:
	ldr r1, gax2_init_p
	bx r1
	.align 2, 0
gax2_init_p:
	.4byte 0x8000000 @ PATCH: gaxtapper will change the value
	thumb_func_end gax2_init

	thumb_func_start gax_irq
gax_irq:
	ldr r0, gax_irq_p
	bx r0
	.align 2, 0
gax_irq_p:
	.4byte 0x8000000 @ PATCH: gaxtapper will change the value
	thumb_func_end gax_irq

	thumb_func_start gax_play
gax_play:
	ldr r0, gax_play_p
	bx r0
	.align 2, 0
gax_play_p:
	.4byte 0x8000000 @ PATCH: gaxtapper will change the value
	thumb_func_end gax_play
	.align 2, 0
