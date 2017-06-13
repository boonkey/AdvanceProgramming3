#pragma once
#include "DataTypes.h"
#include "IBattleshipGameAlgo.h"
#include "Ship.h"

class Board : public BoardData {
private:
	char ***board;
	vector<Ship> ships;

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
	const char*** Board::getSidedBoard(bool sideA);
	const char*** Board::getFullBoard() { return const_cast<const char ***>(board); }
	bool gameOver(bool sideA);
};

int pathExist(string pathname);
vector<string> get_all_files_names_within_folder(string folder, string fileType);