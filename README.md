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
-   [ ] Branching instructions

## Pipelining

-   [x] Scalar pipeline using a simple scoreboard
-   [ ] More complicated scoreboard https://en.wikipedia.org/wiki/Scoreboarding
-   [ ] Instructions which can take multiple cycles within a pipeline stage - required for OoO.
-   [ ] Out of order pipeline
-   [ ] Superscalar pipeline

## Refactor

-   [ ] Simulate hardware modules with F/D/E/M/WB **units** - this should hopefully make superscalar pipelines easier to implement.
    -   [ ] Refactor pipeline logic to be much less messy
    -   [ ] Use instruction memory better so that instructions dont have to be reset
-   [ ] Move scoreboard checkint outside the instruction classes and into the pipeline
-   [ ] CPU runprogram() function separate from main, with an event handler for debug IO, eg regdump() on 'r' key. Will also allow better unit testing
-   [ ] Clean up print statements and move any I want to keep into debug mode

## Nice to haves:

-   [ ] Refactor instruction structure to store instructions in memory
-   [ ] Support risc-v pseudoinstructions
-   [ ] Implement all RV32I base instructions
-   [ ] Unit testing

# References

-   Unit lectures/notes
-   https://github.com/jameslzhu/riscv-card/blob/master/riscv-card.pdf
-   https://www.cl.cam.ac.uk/teaching/1617/ECAD+Arch/files/docs/RISCVGreenCardv8-20151013.pdf
