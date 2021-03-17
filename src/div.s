; /*****************************************************************************
; *
; * Replacement for aeabi standard division functions
; * Using the NUC126 hardware divider
; * 
; *****************************************************************************/

				AREA	|.text|, CODE, READONLY

HDIV			EQU		0x50014000


; Signed division using the hardware divider.
; ~20 cycles

				ALIGN
__aeabi_idivmod	PROC
				ROUT
				EXPORT	__aeabi_idivmod

				LDR		R2, = HDIV
				STR		R0, [ R2, #0 ]
				STR		R1, [ R2, #4 ]
				LDR		R0, [ R2, #8 ]
				LDR		R1, [ R2, #12 ]
				BX		LR

				ENDP

				ALIGN
__aeabi_idiv	PROC
				ROUT
				EXPORT	__aeabi_idiv

				LDR		R2, = HDIV
				STR		R0, [ R2, #0 ]
				STR		R1, [ R2, #4 ]
				LDR		R0, [ R2, #8 ]
				BX		LR

				ENDP


; Unsigned divisions are just signed divisions,
; assuming that divisor and dividend never exceed 2^31-1.
; That's lazy but good enough.

__aeabi_uidivmod	EQU		__aeabi_idivmod
					EXPORT	__aeabi_uidivmod

__aeabi_uidiv		EQU		__aeabi_idiv
					EXPORT	__aeabi_uidiv


; Just in case

	IF {FALSE}
				ALIGN
__aeabi_uidivmod	PROC
				ROUT
				EXPORT	__aeabi_uidivmod

				B		__aeabi_idivmod

				ENDP

				ALIGN
__aeabi_uidiv	PROC
				ROUT
				EXPORT	__aeabi_uidiv

				B		__aeabi_idiv

				ENDP
	ENDIF

				ALIGN

				END
