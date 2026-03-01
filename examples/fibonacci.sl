# Fibonacci sequence — first 10 numbers
# Computes iteratively using register arithmetic

num a = 0
num b = 1
num temp = 0

PRINT a
PRINT b

FOR i = 2 TO 10 THEN
    temp = a + b
    PRINT temp
    a = b
    b = temp
END
