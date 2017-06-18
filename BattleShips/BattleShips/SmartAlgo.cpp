#include "IBattleshipGameAlgo.h"
#include "Board.h"

Configuration config;

static std::vector<IBattleshipGameAlgo *> _instancesVec; //Our Algo collection

//probabilty is 1/chance
#define CONFUSE_CHANCE    4				
#define LOW_PRIOR_CHANCE  20			

class SmartAlgo : public IBattleshipGameAlgo{
public:
	Board PlayersBoard;
	bool haveMoreAttacks = true;
	bool isPlayerA;
	

	//vector<pair<int, int>> listOfAttacks;    // declares a vector of integers
	//vector<pair<int, int>>::iterator nextAttack = listOfAttacks.begin();

	bool targetLocked;						//can kill
	bool enemyFools;						//caught enemy bluffing
	vector<pair<int, int>> normProb;		//all the moves I can do
	vector<pair<int, int>> lowProb;			//moves with low priority
	vector<pair<int, int>> highProb;		//target locked moves list
	vector<pair<int, int>> conf;		//list of bad moves - attacked places
	pair<int, int> target,lastTarget;
	int misses;
	
	enum attackList {
		HIGH = 0,
		NORM = 1,
		LOW  = 2,
		CONF = 3
	};

	struct enemyShip {
		Coordinate initHit = Coordinate(0,0,0);
		vector<Coordinate> confirmedHits;
		vector<Coordinate> suspects;
		enum DIRECTION dir = X; 
	};
	attackList list;
	vector<enemyShip> enemyShips;
	map<int,int> initialEnemyShips; //size : count
public:
	SmartAlgo() : PlayersBoard(NULL) {}
	~SmartAlgo() {}

	void setBoard(const BoardData& board) {
		//TODO kaboom my ships
		PlayersBoard.init(board);
	}

	Coordinate attack() {
		int highScore = 0;
		Coordinate candidate = Coordinate(0,0,0);
		if (!enemyShips.empty()) {
			for (auto &s : enemyShips[0].suspects) { //TODO : check performance -> maybe go over all enemy ships
				if (PlayersBoard.cellAt(s)->score() > highScore) {
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
			map <int,vector<Coordinate>> victims;
			for (loc.depth = 1; loc.depth <= PlayersBoard.depth(); loc.depth++) {
				for (loc.row = 1; loc.row <= PlayersBoard.rows(); loc.row++) { //print line marks also
					for (loc.col = 1; loc.col <= PlayersBoard.cols(); loc.col++) {
						int score = PlayersBoard.cellAt(loc)->score();
						if (victims.find(score) == victims.end()) {
							victims[score].push_back(loc);
						}
						else {
							vector<Coordinate> temp;
							temp.push_back(loc);
							victims.insert(make_pair(score,temp));
						}
					}
				}
			}
			return victims.begin()->second[0];
			//TODO : Check performance
		}
	}

	void setPlayer(int player) {
		isPlayerA = (player == 0);
	}

	void notifyOnAttackResult(int player, Coordinate move, AttackResult result) {
		if (result == AttackResult::Miss) {
			PlayersBoard.kaboom(move);
			for (auto &s : enemyShips) {
				for (auto &c : s.suspects) {
					auto it = find(s.suspects.begin(), s.suspects.end(), move);
					if (it != s.suspects.end()) { // it will not be used anymore
						s.suspects.erase(it);
						alignAttack(s,move);
					}
				}
			}
			return;
		}

		if (result == AttackResult::Hit && ((player == 0) != isPlayerA)) { //Enemy hit
			if (!PlayersBoard.isShipThere(move)) {
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
							initialEnemyShips[(int) s.confirmedHits.size()] --;
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
				cout << "nope, no bugs... don't know why you ask...." << endl;
				return;
			}
			for (auto &s : enemyShips) {
				for (auto &c : s.confirmedHits) {
					if (abs(c.col - move.col) + abs(c.row - move.row) + abs(c.depth - move.depth) == 1) {
						s.confirmedHits.push_back(move);
						auto it = find(s.suspects.begin(), s.suspects.end(), move);
						if (it != s.suspects.end()) { // it will not be used anymore
							s.suspects.erase(it);
						}
						else {
							cout << "something is fishy" << endl;
						}
						alignAttack(s,move);
						PlayersBoard.kaboom(move);
						return;
					}
				}
			}
			initAttack(move);
			PlayersBoard.kaboom(move); // A bug may happen in this function if we start a large ship (3+) and enemy hits it's other end
			return;
		}

		cout << "assuming I sank a fucker: " << (result==AttackResult::Sink) << " | " << player << endl;
		for (auto &s : enemyShips) {
			for (auto &c : s.confirmedHits) {
				if (abs(c.col - move.col) + abs(c.row - move.row) + abs(c.depth - move.depth) == 1) {
					s.confirmedHits.push_back(move);
					PlayersBoard.kaboom(s.confirmedHits);
					initialEnemyShips[(int) s.confirmedHits.size()] --;
					return;
				}
			}
		}
		initialEnemyShips[1]--;
		PlayersBoard.kaboom(move);
		return;
	}



	void initAttack(Coordinate move) {
		enemyShip ship;
		ship.initHit.col = move.col;
		ship.initHit.row = move.row;
		ship.initHit.depth = move.depth;
		ship.confirmedHits.push_back(move);
		Coordinate temp = ship.initHit;
		temp.row--;
		if (PlayersBoard.CoordOnBoard(temp))
			ship.suspects.push_back(temp);
		temp.row += 2;
		if (PlayersBoard.CoordOnBoard(temp))
			ship.suspects.push_back(temp);
		temp.row--;
		temp.col--;
		if (PlayersBoard.CoordOnBoard(temp))
			ship.suspects.push_back(temp);
		temp.col += 2;
		if (PlayersBoard.CoordOnBoard(temp))
			ship.suspects.push_back(temp);
		temp.col--;
		temp.depth--;
		if (PlayersBoard.CoordOnBoard(temp))
			ship.suspects.push_back(temp);
		temp.depth += 2;
		if (PlayersBoard.CoordOnBoard(temp))
			ship.suspects.push_back(temp);
		temp.depth--;
		enemyShips.push_back(ship);
	}


	void alignAttack(enemyShip ship,Coordinate move) {
		
	}

	bool removeFromHigh(int row, int col) {
		auto it = find(highProb.begin(), highProb.end(), make_pair(row, col));
		if (it != highProb.end()) {
			highProb.erase(it);
			return true;
		}
		return false;
	}
 
	bool removeFromNorm(int row, int col) {
		auto it = find(normProb.begin(), normProb.end(), make_pair(row, col));
		if (it != normProb.end()) {
			normProb.erase(it);
			return true;
		}
		return false;
	}

	bool removeFromLow(int row, int col) {
		auto it = find(lowProb.begin(), lowProb.end(), make_pair(row, col));
		if (it != lowProb.end()) {
			lowProb.erase(it);
			return true;
		}
		return false;
	}

	bool removeFromConf(int row, int col) {
		auto it = find(conf.begin(), conf.end(), make_pair(row, col));
		if (it != conf.end()) {
			conf.erase(it);
			return true;
		}
		return false;
	}

	void lockOnTarget(int row, int col) {
		lastTarget = make_pair(row, col);
		if (highProb.size())
			highProb.push_back(make_pair(-2, -2));

		if (removeFromNorm(row + 1, col) || removeFromLow(row + 1, col)) 
			highProb.push_back(make_pair(row + 1, col));
		else
			removeFromConf(row + 1, col);

		if (removeFromNorm(row - 1, col) || removeFromLow(row - 1, col))
			highProb.push_back(make_pair(row - 1, col));
		else
			removeFromConf(row - 1, col);

		if (removeFromNorm(row, col + 1) || removeFromLow(row, col + 1))
			highProb.push_back(make_pair(row, col + 1));
		else
			removeFromConf(row, col + 1);

		if (removeFromNorm(row, col - 1) || removeFromLow(row, col - 1))
			highProb.push_back(make_pair(row, col - 1));
		else
			removeFromConf(row, col - 1);
	}

	void updateLow(int row, int col) {
		if (removeFromNorm(row + 1, col))
			lowProb.push_back(make_pair(row + 1, col));
		else
			removeFromConf(row + 1, col);

		if (removeFromNorm(row - 1, col))
			lowProb.push_back(make_pair(row - 1, col));
		else
			removeFromConf(row - 1, col);

		if (removeFromNorm(row, col + 1))
			lowProb.push_back(make_pair(row, col + 1));
		else
			removeFromConf(row, col + 1);

		if (removeFromNorm(row, col - 1))
			lowProb.push_back(make_pair(row, col - 1));
		else
			removeFromConf(row, col - 1);
	}

	void logAttack(int row, int col) {
		if (removeFromHigh(row, col))
			return;
		if (removeFromNorm(row, col))
			return;
		if (removeFromLow(row, col))
			return;
		if (removeFromConf(row, col))
			return;
	}

	void mergeLow() {
		enemyFools = true;
		if (!lowProb.size())
			return;
		normProb.reserve(normProb.size() + lowProb.size());
		normProb.insert(normProb.end(), lowProb.begin(), lowProb.end());
		lowProb.erase(lowProb.begin(), lowProb.end());
	}

	void clearSurrounding(int row, int col) {
		logAttack(row, col);
		logAttack(row + 1, col);
		logAttack(row - 1, col);
		logAttack(row, col - 1);
		logAttack(row, col + 1);
		if (highProb.size() && highProb[0] == make_pair(-1, -1)) {
			highProb.erase(highProb.begin());
		}
	}

	bool pairInline(pair<int, int> a, pair<int, int> b) {
		return ((a.first == b.first) || (a.second == b.second));
	}

};

IBattleshipGameAlgo* GetAlgorithm() {
	_instancesVec.push_back(new SmartAlgo());
	return _instancesVec[_instancesVec.size() - 1];
}