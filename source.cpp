#include <stdio.h>
#include <iostream>
#include <vector>
#include <limits.h>
#include <array>
#include <sstream>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

using namespace std;

// function declarations
void printBoard(vector<vector<int> >&);
int userMove();
void makeMove(vector<vector<int> >&, int, unsigned int);
void errorMessage(int);
int aiMove();
vector<vector<int> > copyBoard(vector<vector<int> >);
bool winningMove(vector<vector<int> >&, unsigned int);
int scoreSet(vector<unsigned int>, unsigned int);
int tabScore(vector<vector<int> >, unsigned int);
array<int, 2> miniMax(vector<vector<int> >&, unsigned int, int, int, unsigned int);
int heurFunction(unsigned int, unsigned int, unsigned int);

// I'll be real and say this is just to avoid magic numbers
unsigned int NUM_COL = 7; // how wide is the board
unsigned int NUM_ROW = 6; // how tall
unsigned int PLAYER = 1; // player number
unsigned int COMPUTER = 2; // AI number
unsigned int MAX_DEPTH = 5; // the default "difficulty" of the computer controlled AI

bool gameOver = false; // flag for if game is over
unsigned int turns = 0; // count for # turns
unsigned int currentPlayer = PLAYER; // current player

vector<vector<int>> board(NUM_ROW, vector<int>(NUM_COL)); // the game board

/**
 * game playing function
 * loops between players while they take turns
 */
void playGame() {
	printBoard(board); // print initial board
	while (!gameOver) { // while no game over state
		if (currentPlayer == COMPUTER) { // AI move
			makeMove(board, aiMove(), COMPUTER);
		}
		else if (currentPlayer == PLAYER) { // player move
			makeMove(board, userMove(), PLAYER);
		}
		else if (turns == NUM_ROW * NUM_COL) { // if max number of turns reached
			gameOver = true;
		}
		gameOver = winningMove(board, currentPlayer); // check if player won
		currentPlayer = (currentPlayer == 1) ? 2 : 1; // switch player
		turns++; // iterate number of turns
		cout << endl;
		printBoard(board); // print board after successful move
	}
	if (turns == NUM_ROW * NUM_COL) { // if draw condition
		cout << "Draw!" << endl;
	}
	else { // otherwise, someone won
		cout << ((currentPlayer == PLAYER) ? "AI Wins!" : "Player Wins!") << endl;
	}
}

/**
 * function that makes the move for the player
 * @param b - the board to make move on
 * @param c - column to drop piece into
 * @param p - the current player
 */
void makeMove(vector<vector<int> >& b, int c, unsigned int p) {
	// start from bottom of board going up
	for (unsigned int r = 0; r < NUM_ROW; r++) {
		if (b[r][c] == 0) { // first available spot
			b[r][c] = p; // set piece
			break;
		}
	}
}

/**
 * prompts the user for their move
 * and ensures valid user input
 * @return the user chosen column
 */
int userMove() {
	int move = -1; // temporary
	while (true) { // repeat until proper input given
		cout << "Enter a column: ";
		cin >> move; // init move as input
		if (!cin) { // if non-integer
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			errorMessage(1); // let user know
		}
		else if (!((unsigned int)move < NUM_COL && move >= 0)) { // if out of bounds
			errorMessage(2); // let user know
		}
		else if (board[NUM_ROW - 1][move] != 0) { // if full column
			errorMessage(3); // let user know
		}
		else { // if it gets here, input valid
			break;
		}
		cout << endl << endl;
	}
	return move;
}

/**
 * AI "think" algorithm
 * uses minimax to find ideal move
 * @return - the column number for best move
 */
int aiMove() {
	cout << "AI is thinking about a move..." << endl;
	return miniMax(board, MAX_DEPTH, 0 - INT_MAX, INT_MAX, COMPUTER)[1];
}

/**
 * Minimax implementation using alpha-beta pruning
 * @param b - the board to perform MM on
 * @param d - the current depth
 * @param alf - alpha
 * @param bet - beta
 * @param p - current player
 */
array<int, 2> miniMax(vector<vector<int> > &b, unsigned int d, int alf, int bet, unsigned int p) {
	/**
	 * if we've reached minimal depth allowed by the program
	 * we need to stop, so force it to return current values
	 * since a move will never (theoretically) get this deep,
	 * the column doesn't matter (-1) but we're more interested
	 * in the score
	 *
	 * as well, we need to take into consideration how many moves
	 * ie when the board is full
	 */
	if (d == 0 || d >= (NUM_COL * NUM_ROW) - turns) {
		// get current score to return
		return array<int, 2>{tabScore(b, COMPUTER), -1};
	}
	if (p == COMPUTER) { // if AI player
		array<int, 2> moveSoFar = {INT_MIN, -1}; // since maximizing, we want lowest possible value
		if (winningMove(b, PLAYER)) { // if player about to win
			return moveSoFar; // force it to say it's worst possible score, so it knows to avoid move
		} // otherwise, business as usual
		for (unsigned int c = 0; c < NUM_COL; c++) { // for each possible move
			if (b[NUM_ROW - 1][c] == 0) { // but only if that column is non-full
				vector<vector<int> > newBoard = copyBoard(b); // make a copy of the board
				makeMove(newBoard, c, p); // try the move
				int score = miniMax(newBoard, d - 1, alf, bet, PLAYER)[0]; // find move based on that new board state
				if (score > moveSoFar[0]) { // if better score, replace it, and consider that best move (for now)
					moveSoFar = {score, (int)c};
				}
				alf = max(alf, moveSoFar[0]);
				if (alf >= bet) { break; } // for pruning
			}
		}
		return moveSoFar; // return best possible move
	}
	else {
		array<int, 2> moveSoFar = {INT_MAX, -1}; // since PLAYER is minimized, we want moves that diminish this score
		if (winningMove(b, COMPUTER)) {
			return moveSoFar; // if about to win, report that move as best
		}
		for (unsigned int c = 0; c < NUM_COL; c++) {
			if (b[NUM_ROW - 1][c] == 0) {
				vector<vector<int> > newBoard = copyBoard(b);
				makeMove(newBoard, c, p);
				int score = miniMax(newBoard, d - 1, alf, bet, COMPUTER)[0];
				if (score < moveSoFar[0]) {
					moveSoFar = {score, (int)c};
				}
				bet = min(bet, moveSoFar[0]);
				if (alf >= bet) { break; }
			}
		}
		return moveSoFar;
	}
}

/**
 * function to tabulate current board "value"
 * @param b - the board to evaluate
 * @param p - the player to check score of
 * @return - the board score
 */
int tabScore(vector<vector<int> > b, unsigned int p) {
	int score = 0;
	vector<unsigned int> rs(NUM_COL);
	vector<unsigned int> cs(NUM_ROW);
	vector<unsigned int> set(4);
	/**
	 * horizontal checks, we're looking for sequences of 4
	 * containing any combination of AI, PLAYER, and empty pieces
	 */
	for (unsigned int r = 0; r < NUM_ROW; r++) {
		for (unsigned int c = 0; c < NUM_COL; c++) {
			rs[c] = b[r][c]; // this is a distinct row alone
		}
		for (unsigned int c = 0; c < NUM_COL - 3; c++) {
			for (int i = 0; i < 4; i++) {
				set[i] = rs[c + i]; // for each possible "set" of 4 spots from that row
			}
			score += scoreSet(set, p); // find score
		}
	}
	// vertical
	for (unsigned int c = 0; c < NUM_COL; c++) {
		for (unsigned int r = 0; r < NUM_ROW; r++) {
			cs[r] = b[r][c];
		}
		for (unsigned int r = 0; r < NUM_ROW - 3; r++) {
			for (int i = 0; i < 4; i++) {
				set[i] = cs[r + i];
			}
			score += scoreSet(set, p);
		}
	}
	// diagonals
	for (unsigned int r = 0; r < NUM_ROW - 3; r++) {
		for (unsigned int c = 0; c < NUM_COL; c++) {
			rs[c] = b[r][c];
		}
		for (unsigned int c = 0; c < NUM_COL - 3; c++) {
			for (int i = 0; i < 4; i++) {
				set[i] = b[r + i][c + i];
			}
			score += scoreSet(set, p);
		}
	}
	for (unsigned int r = 0; r < NUM_ROW - 3; r++) {
		for (unsigned int c = 0; c < NUM_COL; c++) {
			rs[c] = b[r][c];
		}
		for (unsigned int c = 0; c < NUM_COL - 3; c++) {
			for (int i = 0; i < 4; i++) {
				set[i] = b[r + 3 - i][c + i];
			}
			score += scoreSet(set, p);
		}
	}
	return score;
}

/**
 * function to find the score of a set of 4 spots
 * @param v - the row/column to check
 * @param p - the player to check against
 * @return - the score of the row/column
 */
int scoreSet(vector<unsigned int> v, unsigned int p) {
	unsigned int good = 0; // points in favor of p
	unsigned int bad = 0; // points against p
	unsigned int empty = 0; // neutral spots
	for (unsigned int i = 0; i < v.size(); i++) { // just enumerate how many of each
		good += (v[i] == p) ? 1 : 0;
		bad += (v[i] == PLAYER || v[i] == COMPUTER) ? 1 : 0;
		empty += (v[i] == 0) ? 1 : 0;
	}
	// bad was calculated as (bad + good), so remove good
	bad -= good;
	return heurFunction(good, bad, empty);
}

/**
 * my """heuristic function""" is pretty bad, but it seems to work
 * it scores 2s in a row and 3s in a row
 * @param g - good points
 * @param b - bad points
 * @param z - empty spots
 * @return - the score as tabulated
 */
int heurFunction(unsigned int g, unsigned int b, unsigned int z) {
	int score = 0;
	if (g == 4) { score += 500001; } // preference to go for winning move vs. block
	else if (g == 3 && z == 1) { score += 5000; }
	else if (g == 2 && z == 2) { score += 500; }
	else if (b == 2 && z == 2) { score -= 501; } // preference to block
	else if (b == 3 && z == 1) { score -= 5001; } // preference to block
	else if (b == 4) { score -= 500000; }
	return score;
}

/**
 * function to determine if a winning move is made
 * @param b - the board to check
 * @param p - the player to check against
 * @return - whether that player can have a winning move
 */
bool winningMove(vector<vector<int> > &b, unsigned int p) {
	unsigned int winSequence = 0; // to count adjacent pieces
	// for horizontal checks
	for (unsigned int c = 0; c < NUM_COL - 3; c++) { // for each column
		for (unsigned int r = 0; r < NUM_ROW; r++) { // each row
			for (int i = 0; i < 4; i++) { // recall you need 4 to win
				if ((unsigned int)b[r][c + i] == p) { // if not all pieces match
					winSequence++; // add sequence count
				}
				if (winSequence == 4) { return true; } // if 4 in row
			}
			winSequence = 0; // reset counter
		}
	}
	// vertical checks
	for (unsigned int c = 0; c < NUM_COL; c++) {
		for (unsigned int r = 0; r < NUM_ROW - 3; r++) {
			for (int i = 0; i < 4; i++) {
				if ((unsigned int)b[r + i][c] == p) {
					winSequence++;
				}
				if (winSequence == 4) { return true; }
			}
			winSequence = 0;
		}
	}
	// the below two are diagonal checks
	for (unsigned int c = 0; c < NUM_COL - 3; c++) {
		for (unsigned int r = 3; r < NUM_ROW; r++) {
			for (int i = 0; i < 4; i++) {
				if ((unsigned int)b[r - i][c + i] == p) {
					winSequence++;
				}
				if (winSequence == 4) { return true; }
			}
			winSequence = 0;
		}
	}
	for (unsigned int c = 0; c < NUM_COL - 3; c++) {
		for (unsigned int r = 0; r < NUM_ROW - 3; r++) {
			for (int i = 0; i < 4; i++) {
				if ((unsigned int)b[r + i][c + i] == p) {
					winSequence++;
				}
				if (winSequence == 4) { return true; }
			}
			winSequence = 0;
		}
	}
	return false; // otherwise no winning move
}

/**
 * sets up the board to be filled with empty spaces
 * also used to reset the board to this state
 */
void initBoard() {
	for (unsigned int r = 0; r < NUM_ROW; r++) {
		for (unsigned int c = 0; c < NUM_COL; c++) {
			board[r][c] = 0; // make sure board is empty initially
		}
	}
}

/**
 * function to copy board state to another 2D vector
 * ie. make a duplicate board; used for mutating copies rather
 * than the original
 * @param b - the board to copy
 * @return - said copy
 */
vector<vector<int> > copyBoard(vector<vector<int> > b) {
	vector<vector<int>> newBoard(NUM_ROW, vector<int>(NUM_COL));
	for (unsigned int r = 0; r < NUM_ROW; r++) {
		for (unsigned int c = 0; c < NUM_COL; c++) {
			newBoard[r][c] = b[r][c]; // just straight copy
		}
	}
	return newBoard;
}

/**
 * prints the board to console out
 * @param b - the board to print
 */
void printBoard(vector<vector<int> > &b) {
	for (unsigned int i = 0; i < NUM_COL; i++) {
		cout << " " << i;
	}
	cout << endl << "---------------" << endl;
	for (unsigned int r = 0; r < NUM_ROW; r++) {
		for (unsigned int c = 0; c < NUM_COL; c++) {
			cout << "|";
			switch (b[NUM_ROW - r - 1][c]) {
			case 0: cout << " "; break; // no piece
			case 1: cout << "O"; break; // one player's piece
			case 2: cout << "X"; break; // other player's piece
			}
			if (c + 1 == NUM_COL) { cout << "|"; }
		}
		cout << endl;
	}
	cout << "---------------" << endl;
	cout << endl;
}

/**
 * handler for displaying error messages
 * @param t - the type of error to display
 */
void errorMessage(int t) {
	if (t == 1) { // non-int input
		cout << "Use a value 0.." << NUM_COL - 1 << endl;
	}
	else if (t == 2) { // out of bounds
		cout << "That is not a valid column." << endl;
	}
	else if (t == 3) { // full column
		cout << "That column is full." << endl;
	}
	cout << endl;
}

/**
 * main driver
 */
int main(int argc, char** argv) {
	int i = -1; bool flag = false;
	if (argc == 2) {
		istringstream in(argv[1]);
		if (!(in >> i)) { flag = true; }
		if (i > (int)(NUM_ROW * NUM_COL) || i <= -1) { flag = true; }
		if (flag) { cout << "Invalid command line argument, using default depth = 5." << endl; }
		else { MAX_DEPTH = i; }
	}
	initBoard(); // initial setup
	playGame(); // begin the game
	return 0; // exit state
}

