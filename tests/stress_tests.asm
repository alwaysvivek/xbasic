.text
ldi A 2
mov C A
ldi A 10
mov D A
ldi A 5
mov B D
add
mov B C
sub
sta 128 ; store result from A to x
ldi A 250
sta 129 ; store result from A to a
ldi A 10
sta 130 ; store result from A to b
lda 130 ; load b into A
mov C A
lda 129 ; load a into A
mov B C
add
sta 131 ; store result from A to c
ldi A 5
sta 132 ; store result from A to i
ldi A 5
mov B A
lda 132 ; load i into A
cmp
jne %label_end_1
ldi A 5
mov B A
lda 132 ; load i into A
cmp
jne %label_end_2
ldi A 10
sta 132 ; store result from A to i
label_end_2:
label_end_1:
hlt
