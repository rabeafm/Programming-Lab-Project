;
; thiis is my program:
;
;
MAIN:		add	r3, LIST
Mov:		add	r3, LIST
LOOP:		prn	#48
		lea	STR, r6
		inc	r6
		mov	r3, K
		sub	r1, r4
		bne	END
		cmp	val1, #-6
		bne	%END
		dec	K
		jmp	%LOOP
END:		stop
STR:		.string	"abcd"     
val1:		.string	"abcd"     
STR:		.string	"abcd"     
; what is that
.extern val1
.extern val1
LIST:		.data	+6,5,-7,8,1,-9
LIST:		.data	6, -9
LIST:		.data	6,-9
LIST:		.data	-6,-9
STR:		.string	"abcd"     
STR:		.string	"abcd"    
HTR:		.string	"abcd"
LHOBIST:		.data	6,-9
STR:		.string	"abcd"
LIST:		.data	6,-9
		.data	-100
HOW: .entry  K
K:		.data	31
YELLOW: .extern val1
.extern val1
