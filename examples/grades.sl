# Grade calculator
# Maps a score (0-100) to a grade using ELIF chains

FN grade(score)
    IF score >= 90 THEN
        RETURN 5
    ELIF score >= 80 THEN
        RETURN 4
    ELIF score >= 70 THEN
        RETURN 3
    ELIF score >= 60 THEN
        RETURN 2
    ELSE
        RETURN 1
    END
END

# Grade key: 5=A, 4=B, 3=C, 2=D, 1=F
PRINT grade(95)
# Expected: 5 (A)

PRINT grade(82)
# Expected: 4 (B)

PRINT grade(67)
# Expected: 2 (D)

PRINT grade(45)
# Expected: 1 (F)
