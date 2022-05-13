// Bubblesort benchmark
// t0 = pointer to array start
// t1 = (bool) swapped
// t2 = current pointer
// t3 = current element val
// t4 = next element val
// t5 = magnitude comparison result


// Load in pointer to array start and how long it is
    addi t0, zero, .array

bubblesort:
// Start by wiping t1 and initialising current pointer to .array + 0
    xor     t1, t1, t1
    addi    t2, t0, 0

loop:
// Check if loop should end by comparing current pointer with len, can use a0 because we will never use it otherwise
// load a1 with address off array + current pointer,  load a2 with value of array len, load a2 with address of array + array len
    add     a1, t2, t0
    lw      a2, zero, .len
    addi    a3, a2, .array
//Now if a1 >= a3 we are at the end of the loop
    bge     a1, a3, end
    
// Load in first element slot with the value at the current pointer, second element with current pointer + 1
    lw      t3, t2, 0
    lw      t4, t2, 1
//Branch to loop_tail if current elem < next elem, otherwise swap and set swapped = 1
    blt     t3, t4, loop_end
    sw      t4, t2, 0
    sw      t3, t2, 1
    ori     t1, t1, 1

loop_end:
// increment current pointer
    addi    t2, t2, 1
    beq     zero, zero, loop


end:
// Loop again if a swap was made after a passthrough
    bne     t1, zero, bubblesort 
    hlt





.data

.len 
0x000F

.array
0x000F 0x000E 0x000D 0x000C 0x000B 0x000A 0x0009 0x0008 0x0007 0x0006 0x0005 0x0004 0x0003 0x0002 0x0001