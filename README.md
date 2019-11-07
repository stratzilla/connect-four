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

If C++11 is not an option, you can compile with `-std=c++0x` flag by performing the below:

`$ sed -i 's/moveSoFar = {score, (int)c};/moveSoFar[0] = score; moveSoFar[1] = (int)c;/g' source.cpp`

This changes two lines containing C++11 specific code and replaced with code complilable using C++0x.

# Heuristic Evaluation

The game evaluates the board state by looking at continuous segments of four slots irrespective of their contents. The AI will tabulate piece scores for every single segment of four slots in each direction and create a score as below:

| Friendly Pieces | Opposing Pieces | Empty Spot | Score    |
| --------------- | --------------- | ---------- | -------- |
| 4               | 0               | 0          | +500,001 |
| 3               | 0               | 1          | +5,000   |
| 2               | 0               | 2          | +500     |
| 0               | 2               | 2          | -501     |
| 0               | 3               | 1          | -5,001   |
| 0               | 4               | 0          | -500,000 |

It's a simple heuristic which creates a score based on potential moves, both approaching a win or stopping a win. It will prioritize winning with a 4IAR rather than stopping a 4IAR, but otherwise will prioritize stopping 2IAR/3IAR over making 2IAR/3IAR.

Segments consisting of mixed pieces do not factor into the score as they don't contribute to a loss or a win. Segments consisting of only one piece of either friendly or opposing variety also do not contribute to score as they are not an immediate threat or an immediate method to victory. This is how I play Connect Four, at least.
