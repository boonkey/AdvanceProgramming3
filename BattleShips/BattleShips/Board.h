#pragma once
#include "DataTypes.h"
#include "IBattleshipGameAlgo.h"
#include "Ship.h"

class Board : public BoardData {
private:
	char ***board;
	

public:
	vector<Ship> ships;
	Board(string filename);
	Board(Board& origin);
	void init(Coordinate size) {}
	//Board(Board const &) = delete;
	void operator=(Board const &x) = delete;
	void set(Coordinate loc, char type);
	int loadFromFile(string filename);
	char charAt(Coordinate loc) const override { //return ' ' for non exist coords
		return board[loc.row - 1][loc.col - 1][loc.depth - 1];
	}
	void print();
	Board Board::getSidedBoard(bool sideA);
	const char*** Board::getFullBoard() { return const_cast<const char ***>(board); }
	bool gameOver(bool sideA);
};

int pathExist(string pathname);
vector<string> get_all_files_names_within_folder(string folder, string fileType);