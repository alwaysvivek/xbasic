.text
; Factorial 5! = 120
; Result in res memory address

ldi A 1
sta %res
ldi C 5     ; C is the outer counter (5 down to 2)

outer_loop:
	mov A C
	ldi B 1
	cmp A B
	je %finish  ; if C <= 1, we are done
	
	; Multiply res by C
	lda %res
	mov B A     ; B = res (constant for this iteration)
	
	mov A C
	dec
	mov D A     ; D = multiplier - 1
	jz %outer_next ; if C was 1 (shouldn't happen here)
	
inner_add:
	lda %res
	add A B     ; res = res + original_res
	sta %res
	mov A D
	dec
	mov D A
	jnz %inner_add

outer_next:
	mov A C
	dec
	mov C A
	jmp %outer_loop

finish:
hlt

.data
res = 1
