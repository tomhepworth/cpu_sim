// For input n, if n is even replace with n/2, if odd replace with 3n + 1. 
// This will converge to 1.
// The program should repeat this until 1 is reached
// storing the result at each iteration.

//input n
    addi t0, x0, 27
    addi t1, x0, 0
    addi t3, x0, 1

loop:
    sw t0, t1, 0
    addi t1, t1, 1
    and t2, t3, t0
    beq t2, x0, even

odd:
    add t6, x0, t0
    add t0, t0, t0
    add t0, t0, t6
    addi t0, t0, 1
    beq x0, x0, check_end

even:
    srl t0, t0, t3      

check_end:
    bne t0, t3, loop
    sw t0, t1, 0
    hlt