#pragma once

#include "Ship.h"


class Board : public BoardData {
private:
	Cell ***board;
	bool valid = true;
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
	void print(bool heat = false);
	void set(Coordinate loc, char type);

	pair<int, int> scan_Board();
	void cleanBoard();
	void restoreBoard();
	vector<Ship> scan();
	void scanShip(Coordinate cor, Ship& fillShip);
	void burnShip(Coordinate i, char ch);
	int getCounter(Coordinate i);
	Ship addShip(Coordinate i);
	bool check_depth(Coordinate cor, char ch);
	bool check_row(Coordinate cor, char ch);
	bool check_col(Coordinate cor, char ch);
	void checkAjancencies();
};

int pathExist(string pathname);
vector<string> get_all_files_names_within_folder(string folder, string fileType);