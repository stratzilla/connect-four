# Connect Four

C++ implementation of Connect Four using Alpha-beta pruning Minimax.

# Compilation and Execution

Compile with:

`$ g++ source.cpp -std=c++11 -o cf`

Execute with:

`$ ./cf <arg>`

Where `<arg>` is the depth for minimax. Suggested use case is `<arg>` <img src="https://latex.codecogs.com/gif.latex?\in[1,10]" />, any higher and the algorithm takes too long but this is processor specific. Provide no argument and a default depth of 5 is used.

# Dependencies

- C++ (11 or higher)
- gcc/g++
- GNU/Linux

If you do not have C++11, you can compile with `-std=c++0x` flags instead by switching two lines:

Replace `moveSoFar = {score, (int)c};` on both lines 158 and 177 with `moveSoFar[0] = score; moveSoFar[1] = (int)c;`.

# Heuristic Evaluation

The game evaluates the board state by looking at continuous segments of four slots irrespective of their contents. The AI will tabulate piece scores for every single segment of four slots in each direction and create a score as below:

| Friendly Pieces | Opposing Pieces | Empty Spot | Score    |
| --------------- | --------------- | ---------- | -------- |
| 4               | 0               | 0          | +500,001 |
| 3               | 0               | 1          | +5,000   |
| 2               | 0               | 2          | +500     |
| 1               | 0               | 2          | +0       |
| 0               | 1               | 2          | -0       |
| 0               | 2               | 2          | -501     |
| 0               | 3               | 1          | -5,001   |
| 0               | 4               | 0          | -500,000 |

It's a simple heuristic which creates a score based on potential moves, both approaching a win or stopping a win. It will prioritize winning with a 4IAR rather than stopping a 4IAR, but otherwise will priotize stopping 2IAR/3IAR over making 2IAR/3IAR.
