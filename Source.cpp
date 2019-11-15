/*
Program name: Sudoku Creation Tool
Programmer name: David Faulkner
Description: Will generate a sudoku answer key and then store it in a file
Date: August 9, 2019
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <chrono>
#include "Metrics.h"

/*
Description: Used for randomly choosing values to remove from the board
*/
struct position {
     int vecRow, vecCol;
};

/*
Description: Checks if a value can be inserted into a specific spot in the board
Pre-conditions: Takes the current grid, target row, column, and value as arguments as well as a comparison metric
Post-conditions: Will return true if the value can be inserted at the specified location, else will return false
*/
bool checkAdjacency(const int grid[9][9], const int &row, const int &column, const int &val, int & comp)
{
     int squareRegionStartX, squareRegionEndX, squareRegionStartY, squareRegionEndY;

     //Determine X Bounds
     if (row < 3)
     {
          squareRegionStartX = 0; squareRegionEndX = 3;
     }

     else if (row < 6)
     {
          squareRegionStartX = 3; squareRegionEndX = 6;
     }

     else if (row < 9)
     {
          squareRegionStartX = 6; squareRegionEndX = 9;
     }

     //Determine Y Bounds
     if (column < 3)
     {
          squareRegionStartY = 0; squareRegionEndY = 3;
     }

     else if (column < 6)
     {
          squareRegionStartY = 3; squareRegionEndY = 6;
     }

     else if (column < 9)
     {
          squareRegionStartY = 6; squareRegionEndY = 9;
     }

     //Check Adjacent Row
     for (int i = 0; i < 9; i++)
     {
          comp++;
          if (grid[i][column] == val)
               return false;
     }

     //Check Adjacent Column
     for (int i = 0; i < 9; i++)
     {
          comp++;
          if (grid[row][i] == val)
               return false;
     }

     //Check Adjacent Square
     {
          for (int i = squareRegionStartX; i < squareRegionEndX; i++)
          {
               for (int j = squareRegionStartY; j < squareRegionEndY; j++)
               {
                    comp++;
                    if (grid[i][j] == val)
                         return false;
               }
          }
     }

     return true;
}

/*
Description: Will display the current board
Pre-conditions: Takes the board as input
Post-conditions: Will display the board to the screen and log file
*/
void output(const int grid[9][9], std::ofstream &log) {
     
     std::cout << std::endl;
     log << std::endl;
     
     for (int x = 0; x < 9; ++x) {
          for (int y = 0; y < 9; ++y) {
               std::cout << grid[x][y] << " ";
               log << grid[x][y] << " ";
          }
          std::cout << std::endl;
          log << std::endl;
     }

     std::cout << std::endl;
     log << std::endl;

}

/*
Description: Will generate a partial board that will be solved to generate an answer key
Pre-conditions: Takes initially empty grid and log file as arguments as well as a variable to count comparison metrics
Post-conditions: Will store 5 random valid values in the grid
*/
void createPermutation(int grid[9][9], std::ofstream &log, int &comp) {
     
     int row, column, value;
     int counter = 0;
     bool loop = true;
     
     while (loop) {

          //generate random place and value
          row = rand() % 9;
          column = rand() % 9;
          value = rand() % 9 + 1;

          //check if the space is empty and is valid to place
          if (grid[row][column] == 0 && checkAdjacency(grid, row, column, value, comp)) {

               grid[row][column] = value;
               counter++;
          }

          //number of values in initial board
          if (counter == 15) {
               loop = false;
          }

     }


}

/*
Description: Generates an output file containing the board
Pre-conditions: Takes the grid as an argument as well as the file to write to
Post-conditions: Will generate a file containing only the partial grid in format "row column value"
*/
void generateFile(const int grid[9][9], std::ofstream &outFile) {

     for (int x = 0; x < 9; ++x) {
          for (int y = 0; y < 9; ++y) {
               if (grid[x][y] != 0) {
                    outFile << x << " " << y << " " << grid[x][y] << std::endl;
               }
          }
     }

}

/*
Description: Will find the next "empty" location
Pre-conditions: Takes the current grid, row and column as inputs
Post-conditions: Stores the row and column positions of the next empty position
*/
bool findNextEmpty(const int grid[9][9], int &row, int &column) {
     //find the next position that contains 0
     for (row = 0; row < 9; ++row) {
          for (column = 0; column < 9; ++column) {
               if (!grid[row][column]) {
                    return true;
               }
          }
     }

     return false;
}

/*
Description: Will solve a given puzzle
Pre-conditions: Takes a grid, log file, two booleans to track how much of the graph is completed, and metric variables as arguments
Post-conditions: Grid will return completely solved
*/
bool solver(int grid[9][9], std::ofstream &log, bool &first, bool &second, int &comp, int &back, std::chrono::time_point<std::chrono::system_clock> &one, std::chrono::time_point<std::chrono::system_clock> &two) {

     int row, column;

     //will return when board is finished
     if (!findNextEmpty(grid, row, column)) {
          return true;
     }

     //set time points for solving certain milestones (first and second thirds)
     if (row == 4) {
          one = std::chrono::system_clock::now();
     }
     if (row == 7) {
          two = std::chrono::system_clock::now();
     }

     //print off first and second third of the board
     if (row == 3 && first) {
          std::cout << "First third:";
          log << "First third:";
          output(grid, log);
          first = false;
     }
     if (row == 6 && second) {
          std::cout << "Second third:";
          log << "Second third:";
          output(grid, log);
          second = false;
     }

     //check each value to find one that fits
     for (int value = 1; value < 10; ++value) {
          if (checkAdjacency(grid, row, column, value, comp)) {
               grid[row][column] = value;
               if (solver(grid, log, first, second, comp, back, one, two)) {
                    return true;
               }
               grid[row][column] = 0;
               back++;
          }
     }

     return false;
}

/*
Description: A version of solver that will continue to search for multiple solutions after finding one
Pre-conditions: Will take the board, log file, the row, column, and value of the most recently removed value, and metric trackers for comparisons and backtracks
Post-conditions: Will return true if a solution is found (and thus the board has multiple solutions), or false if no other solution was found
*/
bool hintSolver(int grid[9][9], std::ofstream& log, const int &inRow, const int &inCol, const int &inVal, int& comp, int& back) {

     int row, column;

     //will return when board is finished, indicating a solution has been found
     if (!findNextEmpty(grid, row, column)) {
          return true;
     }

     for (int value = 1; value < 10; ++value) {

          if (row == inRow && column == inCol && value == inVal) {    //skip the value that is already known to be in the solution

               value++;
          }

          if (checkAdjacency(grid, row, column, value, comp)) {  //check if current value can fit in the spot
               
               grid[row][column] = value;
               if (hintSolver(grid, log, inRow, inCol, inVal, comp, back)) {
                    return true;
               }
               grid[row][column] = 0;
               back++;
          }
     }

     return false;
}

/*
Description: Will delete the current index cell from the vector
Pre-conditions: Takes a vector and a position as arguments
Post-conditions: Vector location is overwritten with the end data and then size decreased by one
*/
void swap(std::vector <position> &inVec, const int &index) {

     inVec.at(index).vecRow = inVec.at(inVec.size() - 1).vecRow;
     inVec.at(index).vecCol = inVec.at(inVec.size() - 1).vecCol;

     inVec.pop_back();
}

/*
Description: Will remove values from the board until a partial grid remains with a requested number of hints
Pre-conditions: Takes the answer key, log file, comparison and backtrack metrics, and requested hints as arguments
Post-conditions: Partial grid will remain at end
*/
void partialGridHints(int grid[9][9], std::ofstream &log, int &comp, int &back, const int &inHints) {

     int currRow, currCol, currVal, temp;
     bool loop = true;
     std::vector <position> remainingPositions(81);
     int tempGrid[9][9];

     int iterations = 0;

     //storing list of remaining filled positions on the board
     for (int w = 0; w < 81; ++w) {
          remainingPositions.at(w).vecRow = w / 9;
          remainingPositions.at(w).vecCol = w % 9;
     }


     while (remainingPositions.size() > inHints) {

          //determine position to remove
          temp = rand() % remainingPositions.size();
          
          //store in case of failure to remove
          currRow = remainingPositions.at(temp).vecRow;
          currCol = remainingPositions.at(temp).vecCol;
          currVal = grid[currRow][currCol];

          //remove value
          grid[currRow][currCol] = 0;
          
          //store in temp grid to be solved
          for (int x = 0; x < 9; ++x) {
               for (int y = 0; y < 9; ++y) {
                    tempGrid[x][y] = grid[x][y];
               }
          }

          //ensure there is only one solution
          if (!hintSolver(tempGrid, log, currRow, currCol, currVal, comp, back)) {       //removing value produces only one solution, remove position from possible choices
               swap(remainingPositions, temp);
          }
          else {         //removing value produces more than one solution, reinsert removed value
               grid[currRow][currCol] = currVal;
          }


     }


}


/*
Description: Will generate an answer key for sudoku and then generate a partial grid for future solving
*/
int main() {

     std::ofstream outFile, logFile;
     std::string fileName;
     char userChoice;
     bool choiceLoop = true, firstThird = true, secondThird = true;
     int backtracks = 0, comparisons = 0, remBack = 0, remComp = 0;
     std::chrono::time_point<std::chrono::system_clock> timeOne, timeTwo;

     srand(time(0));
     
     int answerKey[9][9];

     //initialize all to "empty"
     for (int x = 0; x < 9; ++x) {
          for (int y = 0; y < 9; ++y) {
               answerKey[x][y] = 0;
          }
     }

     std::cout << "Welcome to the Sudoku Creation Tool\n\n";

     //log file
     std::cout << "Please enter the name of a file to log actions to: ";
     std::cin >> fileName;
     logFile.open(fileName);
     logFile << "Welcome to the Sudoku Creation Tool\n\n";

     //storage file
     std::cout << "Please enter the name of a file to store the puzzle in: ";
     logFile << "Please enter the name of a file to store the puzzle in: ";
     std::cin >> fileName;
     logFile << fileName << std::endl;
     outFile.open(fileName);

     //choosing difficulties
     std::cout << "Please enter a difficulty level ('e', 'm', 'h'): ";
     logFile << "Please enter a difficulty level ('e', 'm', 'h'): ";
     while (choiceLoop) {
          std::cin >> userChoice;
          if (userChoice != 'e' && userChoice != 'm' && userChoice != 'h') {
               std::cout << "Error, please choose difficulty again: ";
               logFile << "\nError, please choose difficulty again: ";
          }
          else {
               choiceLoop = false;
          }
     }
     logFile << userChoice << std::endl;


     //generating initial board
     createPermutation(answerKey, logFile, comparisons);
     std::cout << "\nInitial board:";
     logFile << "\nInitial board:";
     output(answerKey, logFile);
     std::cout << std::endl;
     logFile << std::endl;

     //solving board
     std::chrono::time_point<std::chrono::system_clock> timeNow = std::chrono::system_clock::now();
     if (!solver(answerKey, logFile, firstThird, secondThird, comparisons, backtracks, timeOne, timeTwo)) {
          std::cout << "Board cannot be found. Ending program.\n";
          logFile << "Board cannot be found. Ending program.\n";
          system("pause");
          return 0;
     }
     std::chrono::time_point <std::chrono::system_clock> timeEnd = std::chrono::system_clock::now();
     std::cout << "Final answer key:";
     logFile << "Final answer key:";
     output(answerKey, logFile);
     

     //removing hints
     switch (userChoice) {
     case 'e': partialGridHints(answerKey, logFile, remComp, remBack, 50);
          break;
     case 'm': partialGridHints(answerKey, logFile, remComp, remBack, 40);
          break;
     case 'h': partialGridHints(answerKey, logFile, remComp, remBack, 30);
          break;
     default:
          std::cout << "\n\nError, switch case is broken.\n\n";
          logFile << "\n\nError, switch case is broken.\n\n";
     }
     
     //displaying partial grid
     std::cout << "Partial grid:";
     logFile << "Partial grid:";
     output(answerKey, logFile);
     std::cout << std::endl;
     logFile << std::endl;

     //storing grid in file
     generateFile(answerKey, outFile);

     //calculating time
     std::chrono::duration<double> firstThirdSolved = timeOne - timeNow;
     std::chrono::duration<double> secondThirdSolved = timeTwo - timeNow;
     std::chrono::duration<double> finalThirdSolved = timeEnd - timeNow;

     //metrics
     std::cout << "Total comparisons made for solver: " << comparisons << std::endl;
     std::cout << "Total backtracks made for solver: " << backtracks << std::endl;
     std::cout << "Total comparisons made for hint generator: " << remComp << std::endl;
     std::cout << "Total backtracks made for hint generator: " << remBack << std::endl;
     std::cout << "Time spent solving first third: " << firstThirdSolved.count() << " seconds" << std::endl;
     std::cout << "Time spent solving second third: " << secondThirdSolved.count() << " seconds" << std::endl;
     std::cout << "Total time spent solving the board: " << finalThirdSolved.count() << " seconds" << std::endl;

     logFile << "Total comparisons made for solver: " << comparisons << std::endl;
     logFile << "Total backtracks made for solver: " << backtracks << std::endl;
     logFile << "Total comparisons made for hint generator: " << remComp << std::endl;
     logFile << "Total backtracks made for hint generator: " << remBack << std::endl;
     logFile << "Time spent solving first third: " << firstThirdSolved.count() << " seconds" << std::endl;
     logFile << "Time spent solving second third: " << secondThirdSolved.count() << " seconds" << std::endl;
     logFile << "Total time spent solving the board: " << finalThirdSolved.count() << " seconds" << std::endl;


     system("pause");
     return 0;
}