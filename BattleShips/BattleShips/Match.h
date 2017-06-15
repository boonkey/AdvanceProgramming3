#pragma once

//#include "Board.h"	# TODO : replace all includes with this one , one board is ready to be used

#include "Board.h"

typedef IBattleshipGameAlgo*(*GetAlgo)();

using namespace std;




class Match {
public:
	int scoreA = 0;
	int scoreB = 0;
	//constr
	Match(string playerAPath, string playerBPath, string boardPath);
	//destr
	~Match() {}
	//other functions
	//	pair<int, int> run_thread_match(string playerAPath, string playerBPath, string boardPath);	//TODO move this function to thread class
	pair<int, int> runMatch();

protected:
	unique_ptr<IBattleshipGameAlgo> playerA;
	unique_ptr<IBattleshipGameAlgo> playerB;
	Board matchBoard;

	bool initplayer(string dllPath, bool isA, const Board& mainGameBoard);
	int do_turn(Board &mainGameBoard, int isTurnB);

};