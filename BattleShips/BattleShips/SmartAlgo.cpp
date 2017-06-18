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

	attackList list;

public:
	SmartAlgo() : PlayersBoard(NULL) {}
	~SmartAlgo() {}

	void setBoard(const BoardData& board) {
		PlayersBoard.init(board);
	}

	Coordinate attack() {
		
	}

	void notifyOnAttackResult(int player, int row, int col, AttackResult result) {
		
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