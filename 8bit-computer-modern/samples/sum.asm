.text
; Sum 1 to N
; a = running total (0)
; b = current N (5)

ldi A 0
sta %sum
ldi B 5

loop:
	lda %sum
	add A B
	sta %sum
	
	mov A B
	dec
	mov B A
	
	jnz %loop

hlt

.data
sum = 0
