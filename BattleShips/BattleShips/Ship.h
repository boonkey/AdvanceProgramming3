#pragma once
#include "DataTypes.h"
#include "Cell.h"
#include "IBattleshipGameAlgo.h"


struct enemyShip {
	Coordinate initHit = Coordinate(0, 0, 0);
	vector<Coordinate> confirmedHits;
	vector<Coordinate> suspects;
	enum DIRECTION dir = UNKNOWN;
};


vector<Coordinate>::iterator myFind(vector<Coordinate> &lookinThis, Coordinate &value);
vector<enemyShip>::iterator myFind(vector<enemyShip> &lookinThis, enemyShip &value);



class Ship {
	char type;			// ship type (letter) - indicates both size and side
	vector<pair<Coordinate, bool>> position; //for each position we keep <Coordinate,alive>
	bool alive;			// true iff ship is alive (not dead)
	DIRECTION dir;		// indicating ship's direction

public:
	//constructor
	Ship(char t, DIRECTION direction);
	bool isSideA();
	char getType() { return type; };
	void print();
	int putInPlace(vector<Coordinate> location);
	bool checkAlive();
	bool checkAttack(Coordinate location);
	bool checkLocation(Coordinate location);
	void setDirection(DIRECTION direction);
	DIRECTION getDirection();
	int getShipScore();
	vector<pair<Coordinate, bool>> location();

	

};