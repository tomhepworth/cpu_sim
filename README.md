# Tom Hepworth ACA coursework

My CPU simulator for the University of Bristol's advanced computer architecture unit. My simulator is heavily based on RISC-V.

# Todo:

## Parser:

-   [x] Parser supporting basic instruction format
-   [x] Parser supporting offset syntax
-   [ ] Parser supporting branching and labels
-   [ ] Parser supporting variables

## ISA Implementation

-   [x] Basic instructions (eg Add, sub)
-   [x] Immediate instructions (eg addi)
-   [x] Memory load/store
-   [ ] Branching instructions

## Pipelining

-   [x] Scalar pipeline using a simple scoreboard
-   [ ] More complicated scoreboard https://en.wikipedia.org/wiki/Scoreboarding
-   [ ] Out of order pipeline
-   [ ] Superscalar pipeline

## Nice to haves:

-   [ ] Refactor instruction structure to store instructions in memory
-   [ ] Support risc-v pseudoinstructions
-   [ ] Implement all RV32I base instructions

# References

-   Unit lectures/notes
-   https://github.com/jameslzhu/riscv-card/blob/master/riscv-card.pdf
-   https://www.cl.cam.ac.uk/teaching/1617/ECAD+Arch/files/docs/RISCVGreenCardv8-20151013.pdf
