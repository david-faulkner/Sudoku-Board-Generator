#ifndef METRICS_H
#define METRICS_H

/*
Description: Contains data values to track the number of comparisons made while solving the grid, the number
             of backtracks made when solving the grid, and keeps track of time values at 3 intervals,
             when the first 1/3 of the puzzle is solved, when the second 1/3 of the puzzle is solved,
             and when the final solution is discovered.
*/

struct metrics {
     int numComparisons = 0;
     int numBackTrack = 0;
     double timeSpentSolving[3] = { 0.0, 0.0, 0.0 };
};

#endif