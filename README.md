# Tom Hepworth ACA coursework

My cycle level CPU simulator for the University of Bristol's advanced computer architecture unit. My simulator based on a RISC-V ISA and implements an extended version of Tomasulos algorithm, with a reorder buffer and 2-bit branch prediction.

# Compile using `make`

A binary called `simulator` will be created. Run it from the project directory

Example command:

`./simulator --mode=ooo --program=programs/bubbleSort.asm -d3 -a3 -l3 -r32 --bp=2BIT`

Add the flag --debug to see what's going on inside the CPU. Add the flag --step with debug to step through execution one cycle at a time

-d controls the number of issues per cycle -a controls the number of ALUs. -l controls the number of memory units. -r controls reorder buffer size. --bp selects branch prediction mode. Options are "2BIT" "AT" (always take) "NT" (never take)

# References

-   Unit lectures/notes
-   https://github.com/jameslzhu/riscv-card/blob/master/riscv-card.pdf
-   https://www.cl.cam.ac.uk/teaching/1617/ECAD+Arch/files/docs/RISCVGreenCardv8-20151013.pdf

# Libraries used:

-   Catch2 (unit tests): https://github.com/catchorg/Catch2

