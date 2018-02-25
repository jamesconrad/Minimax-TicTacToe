#include "tictactoe.h"
#include <memory>

Board::Board(const Board& b)
{
	int size = sizeof(char) * 9;
	victor = b.victor;
	currentTurn = b.currentTurn;
	memcpy_s(board, size, b.board, size);
}

Board::Board()
{
	victor = 'N';
	currentTurn = 'X';
	memset(board, ' ', sizeof(char) * 9);
}

//returns and assigns winning player
char Board::CheckWinner()
{
	/*
	0 | 1 | 2
	----------
	3 | 4 | 5
	----------
	6 | 7 | 8
	*/

	//check for vertical lines
	for (int i = 0; i < 3; i++)
		if (board[i] != ' ' && board[i] == board[i + 3] && board[i + 3] == board[i + 6])
			return victor = board[i];

	//check for horizontal lines
	for (int i = 0; i < 9; i += 3)
		if (board[i] != ' ' && board[i] == board[i + 1] && board[i + 1] == board[i + 2])
			return victor = board[i];

	//check for diagonals
	if (board[0] != ' ' && board[0] == board[4] && board[4] == board[8])
		return victor = board[0];
	if (board[2] != ' ' && board[2] == board[4] && board[4] == board[6])
		return victor = board[2];

	//check for draw
	for (int i = 0; i < 9; i++)
		if (board[i] == ' ') return victor = 'N';

	//return D if we have no spaces, N if no winner
	return victor = 'D';
}

#define b(x,y) board[x] == vs->board[y]

bool Board::CheckMirror(Board* vs)
{
	/*  for vertical mirror
	A | X | A
	B | X | B
	C | X | C
	*/
	if (b(0, 2) && b(3, 5) && b(6, 8) && b(1,1) && b(4,4) && b(7,7))
		return true;

	/* check for horizontal mirror
	A | B | C
	X | X | X
	A | B | C
	*/
	if (b(0, 6) && b(1, 7) && b(2, 8) && b(3, 3) && b(4, 4) && b(5, 5))
		return true;

	/* check for diagonal mirror
	A | B | X
	C | X | B
	X | C | A
	*/
	if (b(0, 8) && b(1, 5) && b(3, 7) && b(2, 2) && b(4, 4) && b(6, 6))
		return true;

	/* check for diagonal mirror
	X | B | A
	B | X | C
	A | C | X
	*/
	if (b(1, 3) && b(2, 6) && b(5, 7) && b(0, 0) && b(4, 4) && b(8, 8))
		return true;


	//not a mirror
	return false;
}

int Board::MakeMove(char player, int position)
{
	if (player != currentTurn)
		return 1;
	if (board[position] != ' ')
		return 2;
	if (position >= 9)
		return 3;
	board[position] = currentTurn;
	currentTurn = currentTurn == 'X' ? 'O' : 'X';
	return 0;
}
