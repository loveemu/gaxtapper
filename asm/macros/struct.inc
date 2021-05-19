	.macro struct_begin
	.struct 0
	.endm

	.macro struct_field name, size
\name:
	.struct \name + \size
	.endm
