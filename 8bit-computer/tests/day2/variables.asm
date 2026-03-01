.text
; a = 5
lda %a
ldi A 5
sta %a

; b = 7
lda %b
ldi A 7
sta %b

; c = a + b
lda %a
mov B A
lda %b
add A B
sta %c

hlt

.data
a = 0
b = 0
c = 0
