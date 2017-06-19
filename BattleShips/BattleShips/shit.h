#pragma once
#include <algorithm>
#include <atomic>
#include <cerrno>
#include <conio.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <cmath>
#include <memory>
#include <mutex>
#include <regex>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <utility> // for std::pair
#include <vector>
#include <Windows.h>
#include "Board.h"

static std::vector<IBattleshipGameAlgo *> _instancesVec; //Our shape collection


class shit : public IBattleshipGameAlgo { //this shit works
public:
	Board PlayersBoard;
	bool isPlayerA;
	vector<enemyShip> enemyShips;
	map<int, int> initialEnemyShips; //size : count
	void setPlayer(int player) override;				// called every time the player changes his order
	void setBoard(const BoardData& board) override;	// called once at the beginning of each new game
	Coordinate attack() override;					// ask player for his move
	void notifyOnAttackResult(int player, Coordinate move, AttackResult result) override; // last move result
	void initAttack(Coordinate move);
	void alignAttack(enemyShip &ship, Coordinate &move, AttackResult result);
	void alignX(enemyShip &ship, Coordinate &move, AttackResult result);
	void alignY(enemyShip &ship, Coordinate &move, AttackResult result);
	void alignZ(enemyShip &ship, Coordinate &move, AttackResult result);
};

