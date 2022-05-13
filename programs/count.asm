//Compare this with an ideal unrolled loop

// count to 1 million
addi    t0, x0, 1000000
xor     t1, t1, t1

loop:
    addi t1, t1, 1
    bne t0, t1, loop
    hlt
