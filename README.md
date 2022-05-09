# Tom Hepworth ACA coursework

My CPU simulator for the University of Bristol's advanced computer architecture unit. My simulator is heavily based on RISC-V.

# Todo:

## Parser:

-   [x] Parser supporting basic instruction format
-   [x] Parser supporting offset syntax
-   [x] Parser supporting branching and labels which can calculate addresses for relative branching
-   [ ] Parser supporting variables

## ISA Implementation

-   [x] Basic instructions (eg Add, sub)
-   [x] Immediate instructions (eg addi)
-   [x] Memory load/store
-   [x] Branching instructions

## Pipelining

-   [x] Scalar pipeline using a simple scoreboard
-   [ ] More complicated scoreboard https://en.wikipedia.org/wiki/Scoreboarding
-   [ ] Instructions which can take multiple cycles within a pipeline stage - required for OoO.
-   [ ] Out of order pipeline
-   [ ] Superscalar pipeline
-   [ ] Operand forwarding

## Refactor

-   [x] Simulate hardware modules with F/D/E/M/WB **units** - this should hopefully make superscalar pipelines easier to implement.
    -   [x] Refactor pipeline logic to be much less messy
    -   [x] Use instruction memory better so that instructions dont have to be reset
-   [x] Proper pipeline flushing
-   [x] Move scoreboard checkint outside the instruction classes and into the pipeline
-   [x] CPU runprogram() function separate from main
-   [x] Event handler for debug IO, eg regdump() on 'r' key. Will also allow better unit testing
-   [x] Clean up print statements and move any I want to keep into debug mode

## Nice to haves:

-   [x] Refactor instruction structure to store instructions in memory
-   [ ] Support pseudoinstructions
-   [ ] Implement all RV32I base instructions
-   [x] Unit testing
-   [ ] ncurses terminal interface

# References

-   Unit lectures/notes
-   https://github.com/jameslzhu/riscv-card/blob/master/riscv-card.pdf
-   https://www.cl.cam.ac.uk/teaching/1617/ECAD+Arch/files/docs/RISCVGreenCardv8-20151013.pdf

# Libraries used:

-   Catch2 (unit tests): https://github.com/catchorg/Catch2

# Notes

-   Collatz benchmark running with tomasulos, first test = 79 cycles
