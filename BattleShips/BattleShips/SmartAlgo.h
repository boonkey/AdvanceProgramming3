#pragma once
#pragma once
#include "IBattleshipGameAlgo.h"
#include "Board.h"

static std::vector<IBattleshipGameAlgo *> _instancesVec; //Our Algo collection

class SmartAlgo : public IBattleshipGameAlgo {
public:
	Board PlayersBoard;
	bool isPlayerA;
	vector<enemyShip> enemyShips;
	map<int, int> initialEnemyShips; //size : count
	SmartAlgo() : PlayersBoard(NULL) {}
	~SmartAlgo() {}
	void setBoard(const BoardData& board) override;
	Coordinate attack() override;
	void setPlayer(int player) override;
	void notifyOnAttackResult(int player, Coordinate move, AttackResult result) override;
	void initAttack(Coordinate move);
	void alignAttack(enemyShip &ship, Coordinate &move, AttackResult result);
	void alignX(enemyShip &ship, Coordinate &move, AttackResult result);
	void alignY(enemyShip &ship, Coordinate &move, AttackResult result);
	void alignZ(enemyShip &ship, Coordinate &move, AttackResult result);
};
