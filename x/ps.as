MAIN:		add	r3, LIST
LOOP:		prn	#48
		lea	STR, r6
		inc	r6
		mov	r3 ,K
		sub	r1 , r4
		bne	END
		cmp	val1, #-6
		bne	%END
		dec	K
		jmp	%LOOP
END:		stop
STR:		.string	"abcd"
LIST:		.data	6,-9
		.data	-100
.entry  K
K:		.data	31
.extern val1
