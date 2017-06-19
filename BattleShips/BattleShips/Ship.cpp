#include "Board.h"

Ship::Ship(char t, DIRECTION direction):type(t),alive(true),dir(direction)	{}

// returns true belongs to player A
bool Ship::isSideA() { return IsCharUpperA(type) != 0; }

void Ship::setDirection(DIRECTION direction) {
	dir = direction;
}

DIRECTION Ship::getDirection() {
	return dir;
}

//puts a ship in place (position is set to true position on board. initialized to all parts alive

int Ship::putInPlace(vector<Coordinate> location) {
	for (auto p : location) {
		pair <Coordinate, bool> temp = make_pair(p, true); //make the pair of position and alive
		position.insert(position.end(), 1, temp);				//insert this into the position
	}
	return 0;
}

//returns true iff ship is alive (has at least one more life)
bool Ship::checkAlive() {
	for (auto p : position)
		if (p.second == true)
			return true;
	alive = false;
	return false;
}

void Ship::print() {
	printf("SHIP type = %c ",type);
	for (auto &loc : position)
		PRINT(loc.first);
	cout << endl;
}

//returns true iff attack hit the ship
bool Ship::checkAttack(Coordinate location) {
	for (unsigned int i = 0; i < position.size(); i++) {
		if (location.col == position[i].first.col && 
			location.row == position[i].first.row && 
			location.depth == position[i].first.depth && 
			position[i].second == true) {

			position[i].second = false;
			return true;
		}
	}
	return false;
}

bool Ship::checkLocation(Coordinate location) {
	for (unsigned int i = 0; i < position.size(); i++) {
		if (location.col == position[i].first.col &&
			location.row == position[i].first.row &&
			location.depth == position[i].first.depth){
			return true;
		}
	}
	return false;
}

int Ship::getShipScore() {
	if ((type == 'b') || (type == 'B')) {
		return 2;
	}
	if ((type == 'p') || (type == 'P')) {
		return 3;
	}
	if ((type == 'm') || (type == 'M')) {
		return 7;
	}
	if ((type == 'd') || (type == 'D')) {
		return 8;
	}
	return 0;
}

vector<pair<Coordinate, bool>> Ship::location() {
	return position;
}

vector<Coordinate>::iterator myFind(vector<Coordinate> &lookinThis, Coordinate &value)
{
	vector<Coordinate>::iterator it = lookinThis.begin();
	while (it != lookinThis.end()) {
		if ((it->row == value.row) && (it->col == value.col) && (it->depth == value.depth)) {
			return it;
		}
		it++;
	}
	return lookinThis.end();
}

vector<enemyShip>::iterator myFind(vector<enemyShip> &lookinThis, enemyShip &value)
{
	vector<enemyShip>::iterator it = lookinThis.begin();
	while (it != lookinThis.end()) {
		if ((it->initHit.row == value.initHit.row) && (it->initHit.col == value.initHit.col) && (it->initHit.depth == value.initHit.depth)) {
			return it;
		}
		it++;
	}
	return lookinThis.end();
}