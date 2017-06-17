#pragma once

#include "IBattleshipGameAlgo.h"
#include "Ship.h"
#include "Cell.h"

class Board : public BoardData {
private:
	Cell ***board;

	//unsafe function. only allows internal excess
	Cell *cellAt(Coordinate c);

public:
	//Board(Board const &) = delete;
	
	Board Board::getSidedBoard(bool sideA);
	Board(Board const &origin);
	Board(string filename);
	bool CoordOnBoard(Coordinate coord);
	bool gameOver(bool sideA);
	char charAt(Coordinate c)const override;
	vector<Ship> ships;
	void init(Coordinate size) {}
	void initSet(Coordinate loc, char type);
	void kaboom(Coordinate loc);
	void kaboom(Ship ship);
	void operator=(Board const &x);
	void print();
	void set(Coordinate loc, char type);

	
};

int pathExist(string pathname);
vector<string> get_all_files_names_within_folder(string folder, string fileType);