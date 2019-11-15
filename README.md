# Sudoku Board Generator
 
This program will generate a Sudoku board of three different difficulties. This program also
tracks different metrics of the generation/solving algorithms. Both the board and log will be
output to files specified by the user.

The program will first generate a board of 15 random numbers that all follow the rules of
Sudoku. This is to ensure that each execution of the program will generate a unique board.
It will then run a backtracking solver algorithm to generate a full board. Once this board
is complete, the program will remove values one at a time until only a certain number of
hints remain that is dependent upon the difficulty chosen. A list of metrics will be
displayed at the end.