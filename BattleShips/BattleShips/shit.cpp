#include "shit.h"

IBattleshipGameAlgo* GetAlgorithm() {
	_instancesVec.push_back(new shit());     // Create new instance and keep it in vector
	return _instancesVec[_instancesVec.size() - 1];   // Return last instance
}

void shit::setBoard(const BoardData& board) {
	PlayersBoard.init(board);
	for (auto &s : PlayersBoard.ships) {
		PlayersBoard.kaboom(s);
		if (initialEnemyShips.find((int)s.location().size()) == initialEnemyShips.end()) {
			initialEnemyShips[(int)s.location().size()]++;
		}
		else {
			initialEnemyShips.insert(make_pair((int)s.location().size(), 1));
		}
	}
}

Coordinate shit::attack() {
	int highScore = 0;
	Coordinate candidate = Coordinate(0, 0, 0);
	if (!enemyShips.empty()) {
		for (auto &s : enemyShips[0].suspects) { //TODO : check performance -> maybe go over all enemy ships
			if (PlayersBoard.cellAt(s)->score() >= highScore) {
				highScore = PlayersBoard.cellAt(s)->score();
				candidate.col = s.col;
				candidate.row = s.row;
				candidate.depth = s.depth;
			}
		}
		return candidate;
	}
	else {
		Coordinate loc = Coordinate(0, 0, 0);
		map <int, vector<Coordinate>> victims;
		for (loc.depth = 1; loc.depth <= PlayersBoard.depth(); loc.depth++) {
			for (loc.row = 1; loc.row <= PlayersBoard.rows(); loc.row++) { ////print line marks also
				for (loc.col = 1; loc.col <= PlayersBoard.cols(); loc.col++) {
					int score = PlayersBoard.cellAt(loc)->score();
					if (victims.find(score) == victims.end()) {
						victims[score].push_back(loc);
					}
					else {
						vector<Coordinate> temp;
						temp.push_back(loc);
						victims.insert(make_pair(score, temp));
					}
				}
			}
		}
		srand((unsigned int) time(NULL));
		int i = rand() % victims.rbegin()->second.size();
		return victims.rbegin()->second[i];
	}
}

void shit::setPlayer(int player) {
	isPlayerA = (player == 0);
}

void shit::notifyOnAttackResult(int player, Coordinate move, AttackResult result) {
	if (result == AttackResult::Miss) {
		PlayersBoard.kaboom(move);
		for (auto &s : enemyShips) {
			auto it = myFind(s.suspects, move);
			if (it != s.suspects.end()) { // it will not be used anymore
				s.suspects.erase(it);
				alignAttack(s, move, result);
			}
		}
		return;
	}

	if (result == AttackResult::Hit && ((player == 0) != isPlayerA)) { //Enemy hit
		if (!PlayersBoard.isShipThere(move)) {	// we enter if when opponent attack himself
			initAttack(move);
			PlayersBoard.kaboom(move);
		}
		return;
	}

	if (result == AttackResult::Sink && ((player == 0) != isPlayerA)) { //Enemy Sink
		if (!PlayersBoard.isShipThere(move)) {
			for (auto &s : enemyShips) {
				for (auto &c : s.confirmedHits) {
					if (abs(c.col - move.col) + abs(c.row - move.row) + abs(c.depth - move.depth) == 1) {
						s.confirmedHits.push_back(move);
						PlayersBoard.kaboom(s.confirmedHits);
						initialEnemyShips[(int)s.confirmedHits.size()] --;
						auto it = myFind(enemyShips, s);
						if (it != enemyShips.end()) { // it will not be used anymore
							enemyShips.erase(it);
						}
						return;
					}
				}
			}
			initialEnemyShips[1]--;
			PlayersBoard.kaboom(move);
		}
		return;
	}

	if (result == AttackResult::Hit) { //We hit
		if (PlayersBoard.isShipThere(move)) {
			return;
		}
		for (auto &s : enemyShips) {
			for (auto &c : s.confirmedHits) {
				if (abs(c.col - move.col) + abs(c.row - move.row) + abs(c.depth - move.depth) == 1) {
					s.confirmedHits.push_back(move);
					auto it = myFind(s.suspects, move);
					if (it != s.suspects.end()) { // it will not be used anymore
						s.suspects.erase(it);
					}
					else {
					}
					alignAttack(s, move, result);
					PlayersBoard.kaboom(move);
					return;
				}
			}
		}
		initAttack(move);
		PlayersBoard.kaboom(move); // A bug may happen in this function if we start a large ship (3+) and enemy hits it's other end
		return;
	}

	for (auto &s : enemyShips) {
		for (auto &c : s.confirmedHits) {
			if (abs(c.col - move.col) + abs(c.row - move.row) + abs(c.depth - move.depth) == 1) {
				s.confirmedHits.push_back(move);
				PlayersBoard.kaboom(s.confirmedHits);
				initialEnemyShips[(int)s.confirmedHits.size()] --;
				auto it = myFind(enemyShips, s);
				if (it != enemyShips.end()) { // it will not be used anymore
					enemyShips.erase(it);
				}
				return;
			}
		}
	}
	initialEnemyShips[1]--;
	PlayersBoard.kaboom(move);
	return;
}



void shit::initAttack(Coordinate move) {
	enemyShip ship;
	ship.initHit.col = move.col;
	ship.initHit.row = move.row;
	ship.initHit.depth = move.depth;
	ship.confirmedHits.push_back(move);
	Coordinate temp = ship.initHit;
	temp.row--;
	if (PlayersBoard.CoordOnBoard(temp))
		ship.suspects.push_back(Coordinate(temp.row,temp.col,temp.depth));
	temp.row += 2;
	if (PlayersBoard.CoordOnBoard(temp))
		ship.suspects.push_back(Coordinate(temp.row,temp.col,temp.depth));
	temp.row--;
	temp.col--;
	if (PlayersBoard.CoordOnBoard(temp))
		ship.suspects.push_back(Coordinate(temp.row,temp.col,temp.depth));
	temp.col += 2;
	if (PlayersBoard.CoordOnBoard(temp))
		ship.suspects.push_back(Coordinate(temp.row,temp.col,temp.depth));
	temp.col--;
	temp.depth--;
	if (PlayersBoard.CoordOnBoard(temp))
		ship.suspects.push_back(Coordinate(temp.row,temp.col,temp.depth));
	temp.depth += 2;
	if (PlayersBoard.CoordOnBoard(temp))
		ship.suspects.push_back(Coordinate(temp.row,temp.col,temp.depth));
	temp.depth--;
	enemyShips.push_back(ship);
}


void shit::alignAttack(enemyShip &ship, Coordinate &move, AttackResult result) {
	if (result == AttackResult::Miss) {
		auto it = myFind(ship.suspects, move);
		if (it != ship.suspects.end()) { // it will not be used anymore
			ship.suspects.erase(it);
		}
		return;
	}
	if (ship.dir == UNKNOWN) {
		if (move.col == ship.initHit.col) {
			ship.dir = Y;
		}
		else if (move.row == ship.initHit.row) {
			ship.dir = X;			
		}
		else {
			ship.dir = Z;
		}
	}
	alignX(ship, move, result);
	alignY(ship, move, result);
	alignZ(ship, move, result);
}

void shit::alignX(enemyShip &ship, Coordinate &move, AttackResult result) {
	if (ship.dir == X) {
		for (auto &h : ship.confirmedHits) {
			Coordinate temp = h;
			temp.row--;
			if (PlayersBoard.CoordOnBoard(temp)) {
				PlayersBoard.kaboom(temp);
				auto it = myFind(ship.suspects, temp);
				if (it != ship.suspects.end())  // it will not be used anymore
					ship.suspects.erase(it);
			}
			temp.row += 2;
			if (PlayersBoard.CoordOnBoard(temp)) {
				PlayersBoard.kaboom(temp);
				auto it = myFind(ship.suspects, temp);
				if (it != ship.suspects.end())  // it will not be used anymore
					ship.suspects.erase(it);
			}
			temp.row--;
			temp.depth--;
			if (PlayersBoard.CoordOnBoard(temp)) {
				PlayersBoard.kaboom(temp);
				auto it = myFind(ship.suspects, temp);
				if (it != ship.suspects.end())  // it will not be used anymore
					ship.suspects.erase(it);
			}
			temp.depth += 2;
			if (PlayersBoard.CoordOnBoard(temp)) {
				PlayersBoard.kaboom(temp);
				auto it = myFind(ship.suspects, temp);
				if (it != ship.suspects.end())  // it will not be used anymore
					ship.suspects.erase(it);
			}
			temp.depth--;
			temp.col++;
			if (PlayersBoard.CoordOnBoard(temp)) {
				auto it = myFind(ship.confirmedHits, temp);
				if (it == ship.confirmedHits.end() && PlayersBoard.cellAt(temp)->score() > 0)  // it will not be used anymore
					ship.suspects.push_back(Coordinate(temp.row,temp.col,temp.depth));
			}
			temp.col-= 2;
			if (PlayersBoard.CoordOnBoard(temp)) {
				auto it = myFind(ship.confirmedHits, temp);
				if (it == ship.confirmedHits.end() && PlayersBoard.cellAt(temp)->score() > 0)  // it will not be used anymore
					ship.suspects.push_back(Coordinate(temp.row,temp.col,temp.depth));
			}
			temp.col++;
		}
	}
}

void shit::alignY(enemyShip &ship, Coordinate &move, AttackResult result) {
	if (ship.dir == Y) {
		for (auto &h : ship.confirmedHits) {
			Coordinate temp = h;
			temp.col--;
			if (PlayersBoard.CoordOnBoard(temp)) {
				PlayersBoard.kaboom(temp);
				auto it = myFind(ship.suspects, temp);
				if (it != ship.suspects.end())  // it will not be used anymore
					ship.suspects.erase(it);
			}
			temp.col += 2;
			if (PlayersBoard.CoordOnBoard(temp)) {
				PlayersBoard.kaboom(temp);
				auto it = myFind(ship.suspects, temp);
				if (it != ship.suspects.end())  // it will not be used anymore
					ship.suspects.erase(it);
			}
			temp.col--;
			temp.depth--;
			if (PlayersBoard.CoordOnBoard(temp)) {
				PlayersBoard.kaboom(temp);
				auto it = myFind(ship.suspects, temp);
				if (it != ship.suspects.end())  // it will not be used anymore
					ship.suspects.erase(it);
			}
			temp.depth += 2;
			if (PlayersBoard.CoordOnBoard(temp)) {
				PlayersBoard.kaboom(temp);
				auto it = myFind(ship.suspects, temp);
				if (it != ship.suspects.end())  // it will not be used anymore
					ship.suspects.erase(it);
			}
			temp.depth--;
			temp.row++;
			if (PlayersBoard.CoordOnBoard(temp)) {
				auto it = myFind(ship.confirmedHits, temp);
				if (it == ship.confirmedHits.end() && PlayersBoard.cellAt(temp)->score() > 0)  // it will not be used anymore
					ship.suspects.push_back(Coordinate(temp.row,temp.col,temp.depth));
			}
			temp.row -= 2;
			if (PlayersBoard.CoordOnBoard(temp)) {
				auto it = myFind(ship.confirmedHits, temp);
				if (it == ship.confirmedHits.end() && PlayersBoard.cellAt(temp)->score() > 0)  // it will not be used anymore
					ship.suspects.push_back(Coordinate(temp.row,temp.col,temp.depth));
			}
			temp.row++;
		}
	}
}

void shit::alignZ(enemyShip &ship, Coordinate &move, AttackResult result) {
	if (ship.dir == Z) {
		for (auto &h : ship.confirmedHits) {
			Coordinate temp = h;
			temp.row--;
			if (PlayersBoard.CoordOnBoard(temp)) {
				PlayersBoard.kaboom(temp);
				auto it = myFind(ship.suspects, temp);
				if (it != ship.suspects.end())  // it will not be used anymore
					ship.suspects.erase(it);
			}
			temp.row += 2;
			if (PlayersBoard.CoordOnBoard(temp)) {
				PlayersBoard.kaboom(temp);
				auto it = myFind(ship.suspects, temp);
				if (it != ship.suspects.end())  // it will not be used anymore
					ship.suspects.erase(it);
			}
			temp.row--;
			temp.col--;
			if (PlayersBoard.CoordOnBoard(temp)) {
				PlayersBoard.kaboom(temp);
				auto it = myFind(ship.suspects, temp);
				if (it != ship.suspects.end())  // it will not be used anymore
					ship.suspects.erase(it);
			}
			temp.col += 2;
			if (PlayersBoard.CoordOnBoard(temp)) {
				PlayersBoard.kaboom(temp);
				auto it = myFind(ship.suspects, temp);
				if (it != ship.suspects.end())  // it will not be used anymore
					ship.suspects.erase(it);
			}
			temp.col--;
			temp.depth++;
			if (PlayersBoard.CoordOnBoard(temp)) {
				auto it = myFind(ship.confirmedHits, temp);
				if (it == ship.confirmedHits.end() && PlayersBoard.cellAt(temp)->score() > 0)  // it will not be used anymore
					ship.suspects.push_back(Coordinate(temp.row,temp.col,temp.depth));
			}
			temp.depth -= 2;
			if (PlayersBoard.CoordOnBoard(temp)) {
				auto it = myFind(ship.confirmedHits, temp);
				if (it == ship.confirmedHits.end() && PlayersBoard.cellAt(temp)->score() > 0)  // it will not be used anymore
					ship.suspects.push_back(Coordinate(temp.row,temp.col,temp.depth));
			}
			temp.depth++;
		}
	}
}
