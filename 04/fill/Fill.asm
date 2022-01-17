// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.
@SCREEN
D=A
@n
M=D

(CHECK)
@KBD
D=M
@FILL
D;JNE
@CLEAR
0;JMP

(FILL)
//If n >= 24576, then do nothing. Go back to check. Else, fill the screen
@KBD
D=A
@n
D=D-M
@CHECK
D;JEQ

//Blacken
@n
A=M
M=-1

//Now go forward in position
@n
M=M+1
@CHECK
0;JMP

(CLEAR)
@SCREEN
D=A-1
@n
D=D-M
@CHECK
D;JEQ

//Now clear
@n
A=M
M=0

//And go back in position
@n
D=M-1
@n
M=D
@CHECK
0;JMP

