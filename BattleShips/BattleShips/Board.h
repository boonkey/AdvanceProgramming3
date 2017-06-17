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
	vector<Ship> ships;
	void initSet(Coordinate loc, char type);
	Board(string filename);
	Board(Board& origin);
	void init(Coordinate size) {}
	//Board(Board const &) = delete;
	void operator=(Board const &x) = delete;
	void set(Coordinate loc, char type);
	
	//int loadFromFile(string filename);	
	void print();
	Board Board::getSidedBoard(bool sideA);
	//const char*** Board::getFullBoard() { return const_cast<const char ***>(board); }
	bool gameOver(bool sideA);
	bool CoordOnBoard(Coordinate coord);
	void kaboom(Coordinate loc);
	void kaboom(Ship ship);
	char Board::charAt(Coordinate c) const override { //return ' ' for non exist coords
		if (!(c.row > 0 && c.row < rows()))
			return ' ';
		if (!(c.col > 0 && c.col < cols()))
			return ' ';
		if (!(c.depth > 0 && c.depth < depth()))
			return ' ';
		return board[c.row - 1][c.col - 1][c.depth - 1].value;
	}
};

int pathExist(string pathname);
vector<string> get_all_files_names_within_folder(string folder, string fileType);