// iterate for 128
// more registers used 
// but dependencies reduced
// by using
    addi    t0, x0, 128
    xor     t1, t1, t1
    addi    t2, t1, 1
    addi    t3, t1, 2
    addi    t4, t1, 3
loop:
    sw      t1, t1, 0
    sw      t2, t2, 0
    sw      t3, t3, 0
    sw      t4, t4, 0
    addi    t1, t1, 4
    addi    t2, t2, 4
    addi    t3, t3, 4
    addi    t4, t4, 4
    bne     t0, t1, loop
    hlt

