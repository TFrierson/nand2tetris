// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
//
// This program only needs to handle arguments that satisfy
// R0 >= 0, R1 >= 0, and R0*R1 < 32768.

// Put your code here.

//x * y code in C:
//for(int i = 1; i < y; i++){
//x += x;

//Create the product variable. Set it to 0
@product
M=0

//We'll create the variable i and set it to 1
@i
M=1

//Compare i to the value in R0. If it is = to it, then we'll go to END.
//Otherwise, we'll go through the loop
(LOOP)
@i
D=M
@R0
D=D-M
@END
D;JGT

@R1
D=M
@product
M=D+M
@i
M=M+1
@LOOP
0;JMP

//Here, take the value in product and put it in R2 like the instructions say
(END)
@product
D=M
@R2
M=D
