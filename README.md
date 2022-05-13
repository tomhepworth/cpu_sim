# Tom Hepworth ACA coursework

My CPU simulator for the University of Bristol's advanced computer architecture unit. My simulator is heavily based on RISC-V.

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

# Notes

-   Collatz benchmark running with 5 stage scalar, first test = 145 cycles
-   Collatz benchmark running with tomasulos, first test = 79 cycles
-   Collatz benchmark running with tomasulos superscalar, 2 decoders, 3 adders, 1 memory, first test = 47 cycles
-
