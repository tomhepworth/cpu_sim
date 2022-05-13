// iterate for 128
    addi    t0, x0, 128
    xor     t1, t1, t1

loop:
//a[i] = i
//a[i + 1] = i + 1
// ...
//a[i + 3] = i + 3
    addi    t2, t1, 1
    addi    t3, t1, 2
    addi    t4, t1, 3
    sw      t1, t1, 0
    sw      t2, t1, 1
    sw      t3, t1, 2
    sw      t4, t1, 3
// i += 4
    addi    t1, t1, 4
    bne     t0, t1, loop
    hlt



    

