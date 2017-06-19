#pragma once

#include "Ship.h"


class Board : public BoardData {
private:
	Cell ***board;
	bool valid = true;

public:
	Board getSidedBoard(bool sideA) const ;
	Coordinate getSize() const;
	Board(Coordinate size);
	Board(string filename);
	Board() {}
	bool isValid();
	bool CoordOnBoard(Coordinate coord);
	bool gameOver(bool sideA);
	char charAt(Coordinate c)const override;
	Cell *cellAt(Coordinate c);
	vector<Ship> ships;
	void init(BoardData const &board);
	void initSet(Coordinate loc, char type);
	int isShipThere(Coordinate location);
	void kaboom(Coordinate loc);
	void kaboom(Ship ship);
	void kaboom(vector<Coordinate> kaboomboom);
	void operator=(Board const &x);
	void print(bool heat = false);
	void print() const;
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