

struct Board
{
	Board();
	Board(const Board& b);
	int MakeMove(char player, int position);//updates board, and current turn
	bool CheckMirror(Board* vs);//returns true if this board is a mirror of vs
	char CheckWinner();//returns the winning character, 'D' for a draw, and 'N' if there are moves left
	char board[9];//1d array representing 2d board
	char victor;//winning player
	char currentTurn;//current turn
};