;/******************************************************************************
; *
; * @file	aegis.s
; *
; * @brief	Assembly procedures
; *
; *****************************************************************************/

				AREA	|.text|, CODE, READONLY


; Integer square root (Newton's method).
; Average execution time ~120 cycles, using hardware mul and div.

HDIV			EQU		0x50014000

				ALIGN
isqrt			PROC
				ROUT
				EXPORT	isqrt

				PUSH	{R4,R5}
				FRAME PUSH {R4,R5}

				MOV		R1, R0
				LDR		R4, = HDIV
				LDR		R0, = 2500
0				MOV		R2, R0
				MULS	R2, R2
				ADD		R2, R1
				LSLS	R3, R0, #1
				STR		R2, [R4, #0]
				STR		R3, [R4, #4]
				LDR		R3, [R4, #8]
				LDR		R5, [R4, #12]
				CMP		R5, R0
				BLT		%1
				ADDS	R3, #1
1				CMP		R0, R3
				MOV		R0, R3
				BNE		%0

				POP		{R4,R5}
				FRAME POP {R4,R5}

				BX		LR

				ENDP


	IF {FALSE}

; Another integer square root routine without hardware multiplier or divider.
; (Pseudo-Euclidian division)
; ~250 cycles.

				ALIGN
isqrt			PROC
				ROUT
				EXPORT	isqrt

				MOV		R1, R0		; r1 = num
				MOVS	R0, #0		; r0 = res
				MOVS	R2, #1
				LSLS	R2, #30		; r2 = bit

0				CMP		R2, R1
				BLE		%1
				LSRS	R2, #2
				BNE		%0
				B		%2

1				ADDS	R3, R0, R2
				CMP		R1, R3
				BLT		%3
				SUBS	R1, R3
				LSRS	R0, #1
				ADDS	R0, R2
				B		%4
3				LSRS	R0, #1

4				LSRS	R2, #2
				BNE		%1
2
				BX		LR

				ENDP

	ENDIF


; Microseconds delay.
; Total cycles = 72 * R0
; (accounting for the 6 cycles of the call, MOVS R0,#xx + BL WaitUs)
; Excess time is 1.6 * R0 nanoseconds given that CLK = 71.8848MHz, not 72.
; Drift is below 1us for delays below 624us. Good enough.
; Minimun 0.21µs for R0 = 0. Don't call this function if you don't expect to wait.

				ALIGN
WaitUs			PROC
				ROUT
				EXPORT	WaitUs

				MOVS	R1, #18			; 1
				MULS	R0, R1, R0		; 1
				SUBS	R0, #4			; 1
				BCC		%1				; 3/1
				NOP						; 1
0				SUBS	R0, #1			; 1
				BCS		%0				; 3/1
1				BX		LR				; 3

				ENDP


				ALIGN

				END
