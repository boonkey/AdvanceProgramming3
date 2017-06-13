#include "Board.h"

Ship::Ship(char t, DIRECTION direction):
	type(t),
	alive(true),
	dir(direction)
	{}

// returns true belongs to player A
bool Ship::isSideA() { return IsCharUpperA(type) != 0; }

void Ship::setDirection(DIRECTION direction) {
	dir = direction;
}

DIRECTION Ship::getDirection() {
	return dir;
}


//void Ship::print() {
//	if (config.quiet)
//		return;
//	cout << "type: " << type << " alive: " << alive << " Side A: " << isSideA() << endl;
//	for (auto p : position) {
//		cout << p.first.col << " , " << p.first.row << " , " << p.first.depth << " # status: " << p.second << endl;
//	}
//}

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
/*
//a ship constructor
Ship shipScan(char value, bool vert, pair<int, int> topLeft, int shipLen, Board& gameBoard) {
	Ship temp(value, vert);
	vector<pair<int, int>> location;
	for (int i = 0; i < shipLen; i++) {
		if (vert)
			location.insert(location.begin(), 1, make_pair(topLeft.first, topLeft.second + i));
		else
			location.insert(location.begin(), 1, make_pair(topLeft.first + i, topLeft.second));
	}
	temp.putInPlace(location);
	burnShip(value, topLeft.first, topLeft.second, gameBoard, value + 1);
	return temp;
}

int verifyValidShape(Board &gameBoard, Board& intermediateBoard, char cmp_val, int shiplen, bool vert, int col, int row) {
	//valid ship=0, invalid =-1 and burn it
	if (shiplen == 0) {
		if (intermediateBoard.get(col + 1, row) == cmp_val || \
			(vert && intermediateBoard.get(col - 1, row) == cmp_val) || \
			(!vert && intermediateBoard.get(col, row - 1) == cmp_val) || \
			intermediateBoard.get(col, row + 1) == cmp_val)	//this needs a quick fix, figure out not to check both top and left, just one
		{
			burnShip(cmp_val, col, row, intermediateBoard); // need to look for all adacent similair letters and replace with x's in intermediate-recursive is advised
			return -1;
		}
		return 0;//success is reached, we are at len=0 and our ship fits exactly
	}
	if (shiplen > 0) //still need to investigate the remaining (expected) length of the ship
	{

		if ((!vert && intermediateBoard.get(col + 1, row) != cmp_val) || \
			(vert && intermediateBoard.get(col, row + 1) != cmp_val)) {//this means the ship is too short, expected more of the ship, yet there is nothing

			burnShip(cmp_val, col, row, intermediateBoard); // need to look for all adacent similair letters and replace with x's in intermediate-recursive is advised
			return -1;
		}
	}

	if (vert) {//should go down

		if (intermediateBoard.get(col, row) == 'o' && shiplen > 0) {//the invalid value of escaping array bounds

			burnShip(cmp_val, col, row - 1, intermediateBoard);
			return -1;
		}

		if (intermediateBoard.get(col - 1, row) == cmp_val || intermediateBoard.get(col + 1, row) == cmp_val) {

			burnShip(cmp_val, col, row, intermediateBoard);
			return -1;
		}
		return (verifyValidShape(gameBoard, intermediateBoard, cmp_val, shiplen - 1, vert, col, row + 1));
	}
	else {//now we should go right

		if (intermediateBoard.get(col, row) == 'o' && shiplen > 0) {//the invalid value of escaping array bounds

			burnShip(cmp_val, col - 1, row, intermediateBoard);
			return -1;
		}
		if (intermediateBoard.get(col, row - 1) == cmp_val || intermediateBoard.get(col, row + 1) == cmp_val) {

			//			intermediateBoard.print();
			burnShip(cmp_val, col, row, intermediateBoard);
			return -1;
		}
		return (verifyValidShape(gameBoard, intermediateBoard, cmp_val, shiplen - 1, vert, col + 1, row));
	}

	return ERR_UNREACHABLE;
}

void burnShip(char cmp_val, int col, int row, Board& intermediateBoard, char temp) {//function to replace ship representation in intermediate board
	if (intermediateBoard.get(col, row) != cmp_val) return; //check all the squares containing this shp's letter
	if (intermediateBoard.set(col, row, temp)) return;       //turn them to x's when done
	burnShip(cmp_val, col + 1, row, intermediateBoard, temp);	//do for all ajacent squares 
	burnShip(cmp_val, col - 1, row, intermediateBoard, temp);
	burnShip(cmp_val, col, row + 1, intermediateBoard, temp);
	burnShip(cmp_val, col, row - 1, intermediateBoard, temp);
	return;
}

// checks if ship (which it's top left is in <row,col>) is valid, and adds it to the board
int topLeftOfShip(Board &gameBoard, Board& intermediateBoard, int col, int row) {
	bool vert = true;
	int shiplen;
	char cmp_val = intermediateBoard.get(col, row);
	switch (cmp_val) {
	case 'b':
	case 'B':
		shiplen = 1;
		break;
	case 'p':
	case 'P':
		shiplen = 2;
		break;
	case 'd':
	case 'D':
		shiplen = 4;
		break;
	case 'm':
	case 'M':
		shiplen = 3;
		break;
	}
	if (intermediateBoard.get(col + 1, row) == cmp_val)
		vert = false;

	if (!verifyValidShape(gameBoard, intermediateBoard, cmp_val, shiplen - 1, vert, col, row)) {  //verifyValidShape needs shiplen-1 to check
		gameBoard.addShip(shipScan(cmp_val, vert, make_pair(col, row), shiplen, intermediateBoard));
		return 0;
	}
	return -1;
}

*/