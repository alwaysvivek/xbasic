# Even or odd checker
# Uses a function to classify numbers

FN is_even(n)
    # Check if n is divisible by 2
    # Trick: subtract 2 repeatedly until 0 or 1
    num val = n
    WHILE val > 1
        val = val - 2
    END
    # val == 0 means even, val == 1 means odd
    IF val == 0 THEN
        RETURN 1
    ELSE
        RETURN 0
    END
END

# Test with several numbers
PRINT is_even(4)
# Expected: 1 (even)

PRINT is_even(7)
# Expected: 0 (odd)

PRINT is_even(10)
# Expected: 1 (even)
