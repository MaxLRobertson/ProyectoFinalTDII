.text
.arm
.global chain2int
		
chain2int:
		
	MOV		R2, R0
	MOV		R0, #0
	MOV     R1, #8
		
for:	
	SUB		R1, R1, #1
	LDR		R3, [R2], #1
	ORR		R0, R0, R3, LSL R1
	CMP		R1, #0
	BGE		for
	MOV		PC, LR
	
