.text
; Fibonacci Sequence (up to 7 values)
; f0 = 0, f1 = 1
; f2 = f0 + f1 = 1
; f3 = f1 + f2 = 2
; ...

ldi A 0
sta %f0
ldi A 1
sta %f1

ldi C 5 ; calculate 5 more values

loop:
	lda %f0
	mov B A
	lda %f1
	add A B ; A = f0 + f1
	sta %next
	
	; Shift: f0 = f1, f1 = next
	lda %f1
	sta %f0
	lda %next
	sta %f1
	
	mov A C
	dec
	mov C A
	jnz %loop

hlt

.data
f0 = 0
f1 = 0
next = 0
