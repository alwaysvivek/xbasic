.text
; if (a == b) { c = c + 1 }
lda %a
ldi A 5
sta %a
lda %b
ldi A 5
sta %b
lda %c
ldi A 10
sta %c

lda %a
mov B A
lda %b
cmp A B
jne %end

lda %c
inc A
sta %c

end:
hlt

.data
a = 0
b = 0
c = 0
