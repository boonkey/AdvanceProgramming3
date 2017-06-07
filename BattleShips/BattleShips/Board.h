#pragma once
#include "DataTypes.h"
#include "IBattleshipGameAlgo.h"
/*
class Ship {
	char type;			// ship type (letter) - indicates both size and side
	vector<pair<pair<int, int>, bool>> position; //for each position we keep <<x,y>,alive>
	bool alive;			// true iff ship is alive (not dead)
	bool horizontal;	// true iff ship is horizontal

public:
	//constructor
	Ship(char t, bool hor);
	bool isSideA();
	void print();
	int putInPlace(vector<pair<int, int>> location);
	bool checkAlive();
	bool checkAttack(pair<int, int> location);
	bool checkLocation(pair<int, int> location);
	int getShipScore();
};
*/
class Board : public BoardData {
private:
	char ***board;

public:
	Board(string filename);
	void init(Coordinate size) {}
	//Board(Board const &) = delete;
	void operator=(Board const &x) = delete;
	void set(Coordinate loc, char type);
	int loadFromFile(string filename);
	char charAt(Coordinate loc) const override {
		return board[loc.row - 1][loc.col - 1][loc.depth - 1];
	}
	void print();

};

