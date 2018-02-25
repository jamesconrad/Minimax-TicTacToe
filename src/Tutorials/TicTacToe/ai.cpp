#include "ai.h"

int AI::Minimax(Board b, char aiChar, char playerChar)
{
	if (useAlphaBeta)
		return MinimaxAlphaBeta(b, aiChar, playerChar, -1000, 1000);
	return MinimaxStandard(b, aiChar, playerChar);
}

int AI::MinimaxAlphaBeta(Board b, char aiChar, char playerChar, int alpha, int beta)
{
	b.CheckWinner();
	//terminal conditions
	if (b.victor == aiChar)
		return 1;

	if (b.victor == playerChar)
		return -1;

	if (b.victor == 'D')
		return 0;

	std::vector<int> openSpaces;
	//calculate all open spaces on the board
	for (int i = 0; i < 9; i++)
		if (b.board[i] == ' ')	openSpaces.push_back(i);

	//for each valid play
	for (int i = 0, s = openSpaces.size(); i < s; i++)
	{
		if (b.currentTurn == aiChar)//max
		{
			Board tmp = b;
			tmp.MakeMove(aiChar, openSpaces[i]);

			int max = MinimaxAlphaBeta(tmp, aiChar, playerChar, alpha, beta);
			if (max > alpha)//update max
				alpha = max;

			if (alpha >= beta)//break if weve already got a good play
				break;
		}
		else if (b.currentTurn == playerChar)//min
		{
			Board tmp = b;
			tmp.MakeMove(playerChar, openSpaces[i]);

			int min = MinimaxAlphaBeta(tmp, aiChar, playerChar, alpha, beta);
			if (min < beta)//update min
				beta = min;

			if (alpha >= beta)//break if weve already got a good play
				break;
		}
	}

	return b.currentTurn == aiChar ? alpha : beta;//return the correct min/max
}

int AI::MinimaxStandard(Board b, char aiChar, char playerChar)
{
	b.CheckWinner();
	//terminal conditions
	if (b.victor == aiChar)
		return 1;

	if (b.victor == playerChar)
		return -1;

	if (b.victor == 'D')
		return 0;

	std::vector<int> openSpaces;
	//calculate all open spaces on the board
	for (int i = 0; i < 9; i++)
		if (b.board[i] == ' ')	openSpaces.push_back(i);

	//set score to min or max based on turn
	int score = b.currentTurn == aiChar ? -1000 : 1000;
	for (int i = 0, s = openSpaces.size(); i < s; i++)
	{
		if (b.currentTurn == aiChar)//max
		{
			Board tmp = b;
			tmp.MakeMove(aiChar, openSpaces[i]);

			int max = Minimax(tmp, aiChar, playerChar);
			if (max > score)
				score = max;
		}
		else if (b.currentTurn == playerChar)//min
		{
			Board tmp = b;
			tmp.MakeMove(playerChar, openSpaces[i]);

			int min = Minimax(tmp, aiChar, playerChar);
			if (min < score)
				score = min;
		}
	}

	return score;//return min/max
}

int AI::BestMove(Board b)
{
	if (useMirrorDropout)
		return BestMoveMirror(b);
	return BestMoveStandard(b);
}

int AI::BestMoveMirror(Board b)
{
	std::vector<int> openSpaces;
	//calculate all open spaces on the board
	for (int i = 0; i < 9; i++)
		if (b.board[i] == ' ')	openSpaces.push_back(i);

	int max = -1000;

	//determine and make all possible moves
	std::vector<Board*> possibleMoves;
	for (int i = 0, s = openSpaces.size(); i < s; i++)
	{
		Board* tmp = new Board(b);
		tmp->MakeMove(tmp->currentTurn, openSpaces[i]);
		possibleMoves.push_back(tmp);
	}


	for (int i = 0, s = possibleMoves.size(); i < s - 1; i++)
	{
		//incase weve already deleted this move
		if (possibleMoves[i] == nullptr)
			continue;
		for (int j = i + 1; j < s; j++)
		{
			//incase weve already deleted this move
			if (possibleMoves[j] == nullptr)
				continue;
			//check if the move is a mirror of any boards after it in the list, prevents double checking
			if (possibleMoves[i]->CheckMirror(possibleMoves[j]))
			{
				//destroy and nullptr the mirrord board
				free(possibleMoves[j]);
				possibleMoves[j] = nullptr;
			}
		}
	}

	//setup move storage
	std::vector<std::pair<int, int>> validMoves(possibleMoves.size());
	for (int i = 0, s = possibleMoves.size(); i < s; i++)
	{
		//skip if this move was deleted by mirror
		if (possibleMoves[i] == nullptr)
			continue;
		//calculate and store score/move
		int mm = Minimax(*possibleMoves[i], b.currentTurn, b.currentTurn == 'X' ? 'O' : 'X');
		validMoves[i] = std::pair<int, int>(openSpaces[i], mm);
		if (mm > max) 
			max = mm;
	}
	
	//fetch a random move, if invalid, take the one after it until valid
	int index = rand() % validMoves.size();
	while (validMoves[index].second != max)
	{
		index++;
		if (index >= validMoves.size())
			index = 0;
	}

	return validMoves[index].first;
}

int AI::BestMoveStandard(Board b)
{
	std::vector<int> openSpaces;
	//calculate all open spaces on the board
	for (int i = 0; i < 9; i++)
		if (b.board[i] == ' ')	openSpaces.push_back(i);

	int max = -1000;

	//setup move storage
	std::vector<std::pair<int, int>> validMoves(openSpaces.size());
	for (int i = 0, s = openSpaces.size(); i < s; i++)
	{
		Board tmp = b;
		//make a move
		tmp.MakeMove(tmp.currentTurn, openSpaces[i]);
		//calculate and store score/move
		int mm = Minimax(tmp, b.currentTurn, b.currentTurn == 'X' ? 'O' : 'X');
		validMoves[i] = std::pair<int, int>(openSpaces[i], mm);
		if (mm > max)
			max = mm;
	}

	//fetch a random move, if invalid, take the one after it until valid
	int index = rand() % validMoves.size();
	while (validMoves[index].second != max)
	{
		index++;
		if (index >= validMoves.size())
			index = 0;
	}

	return validMoves[index].first;
}