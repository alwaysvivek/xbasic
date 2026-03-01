.text
; Parity check for number 13 (0x0D = 1101)
; Result 1 if odd, 0 if even

ldi A 13
ldi B 1
and A B
sta %is_odd
hlt

.data
is_odd = 0
