#pragma once
#include "Board.h"

typedef IBattleshipGameAlgo*(*GetAlgo)();

using namespace std;




class Match {
public:
	int scoreA = 0;
	int scoreB = 0;
	//constr
	Match(string playerAPath, string playerBPath, Board &board);
	//destr
	~Match() {}
	//other functions
	pair<int, int> runMatch();

protected:
	unique_ptr<IBattleshipGameAlgo> playerA;
	unique_ptr<IBattleshipGameAlgo> playerB;
	Board matchBoard;

	bool initplayer(string dllPath, bool isA, const Board& mainGameBoard);
	int do_turn(Board &mainGameBoard, int isTurnB);

};