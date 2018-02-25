#include "tictactoe.h"
#include <vector>

class AI
{
public:
	//optimization flags
	bool useMirrorDropout = false;
	bool useAlphaBeta = false;

	//entry point for minimax
	int BestMove(Board b);
private:
	int BestMoveMirror(Board b);//called if mirror dropout enabled
	int BestMoveStandard(Board b);//called if mirror dropout disabled
	int Minimax(Board b, char aiChar, char playerChar);//calls variants based on alpha beta flag
	int MinimaxStandard(Board b, char aiChar, char playerChar);
	int MinimaxAlphaBeta(Board b, char aiChar, char playerChar, int alpha, int beta);

};