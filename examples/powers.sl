# Powers of 2 — computes 2^0 through 2^7
# Demonstrates doubling pattern within 8-bit range (max 255)

num power = 1

FOR i = 0 TO 7 THEN
    PRINT power
    power = power + power
END

# Expected output:
# 1, 2, 4, 8, 16, 32, 64, 128
