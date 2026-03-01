# Find the maximum of three numbers
# Demonstrates nested IF/ELIF/ELSE

FN max3(a, b)
    IF a > b THEN
        RETURN a
    ELSE
        RETURN b
    END
END

num x = 42
num y = 87
num z = 23

# Compare x and y first, then winner vs z
num winner = max3(x, y)
num result = max3(winner, z)

PRINT result
# Expected: 87
