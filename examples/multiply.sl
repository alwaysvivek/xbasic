# Multiplication via repeated addition
# The 8-bit CPU has no MUL instruction — this implements it in software

FN multiply(a, b)
    num result = 0
    FOR i = 0 TO b THEN
        result = result + a
    END
    RETURN result
END

PRINT multiply(6, 7)
# Expected: 42

PRINT multiply(15, 3)
# Expected: 45

PRINT multiply(0, 100)
# Expected: 0
