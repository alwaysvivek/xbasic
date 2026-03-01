.text
ldi A 12
sta 128 ; store result from A to a
ldi A 9
sta 129 ; store result from A to b
lda 128 ; load a into A
sta 130 ; store result from A to max
ldi A 0
mov B A
lda 128 ; load a into A
mov C A
lda 129 ; load b into A
mov B C
sub
cmp
jne %label_end_1
lda 129 ; load b into A
sta 130 ; store result from A to max
label_end_1:
hlt
