.text
ldi A 0
sta 128 ; init i
ldi A 0
sta 129 ; init sum
ldi A 1
sta 128
label_for_start_1:
lda 128 ; load i into A
mov C A
lda 129 ; load sum into A
mov B C
add
sta 129 ; store result to sum
lda 128
inc
sta 128
mov B A
ldi A 5
cmp ; end - var
jnc label_for_start_1 ; loop if end >= var
lda 129 ; load sum into A
out 0 ; print A to primary I/O
hlt
