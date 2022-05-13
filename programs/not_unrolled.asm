// iterate for 128
addi    t0, x0, 128
xor     t1, t1, t1

loop:
//a[i] = i
    sw      t1, t1, 0
// i++
addi    t1, t1, 1
    bne     t0, t1, loop
    hlt



