#pragma once
#include "DataTypes.h"



class Ship {
	char type;			// ship type (letter) - indicates both size and side
	vector<pair<Coordinate, bool>> position; //for each position we keep <Coordinate,alive>
	bool alive;			// true iff ship is alive (not dead)
	DIRECTION dir;		// indicating ship's direction

public:
	//constructor
	Ship(char t, DIRECTION direction);
	bool isSideA();
	void print();
	int putInPlace(vector<Coordinate> location);
	bool checkAlive();
	bool checkAttack(Coordinate location);
	bool checkLocation(Coordinate location);
	void setDirection(DIRECTION direction);
	DIRECTION getDirection();
	int getShipScore();
};