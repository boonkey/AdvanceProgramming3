#include "Board.h"
//this function goes over all cells, and verifies board legallity ->abnormally shaped/sized ships are earased before checking for ajacent ships, as was required.
//all thips are added to the game manager's array
pair<int, int> Board::scan_Board() {
	cleanBoard();
	int shipCounterA = 0;
	int shipCounterB = 0;
	ships = scan();
	restoreBoard();
	checkAjancencies();
	for (auto item : ships) {
		if (item.isSideA()) {
			shipCounterA++;
		}
		else {
			shipCounterB++;
		}
	}
	pair<int,int> result = make_pair(shipCounterA, shipCounterB);
	return result;
}


//this function clears the board of none-matching characters
void Board::cleanBoard() {
	Coordinate i = Coordinate(0, 0, 0);
	for (i.row = 1; i.row <= rows(); i.row++) {
		for (i.col = 1; i.col <= cols(); i.col++) {
			for (i.depth = 1; i.depth <= depth(); i.depth++) {
				switch(charAt(i)) {
					case 'b':
					case 'B':
					case 'm':
					case 'M':
					case 'd':
					case 'D':
					case 'p':
					case 'P':
						continue;
						break;
					default: set(i, ' ');
				}
			}
		}
	}
}
//TODO check ajacent ships and check that burn_ship kills the ships AND resets valid
//TODO : if the valid flag if false, than leave the board alone
//this function restores ships after scan ruined them
void Board::restoreBoard() {
	Coordinate i = Coordinate(0, 0, 0);
	for (i.row = 1; i.row <= rows(); i.row++) {
		for (i.col = 1; i.col <= cols(); i.col++) {
			for (i.depth = 1; i.depth <= depth(); i.depth++) {
				switch(charAt(i)) {
					case ' ':
						continue;
						break;
					default:
						set(i, static_cast<char>(charAt(i) - 1));
				}
			}
		}
	}
}
void Board::checkAjancencies() {
	Coordinate i = Coordinate(0, 0, 0);
	for (i.row = 1; i.row <= rows(); i.row++) {
		for (i.col = 1; i.col <= cols(); i.col++) {
			for (i.depth = 1; i.depth < depth(); i.depth++) {
				char ch = charAt(i); //we check the char in (i)
				i.col++;             //examine neighboring cols and if we have a different none ' ' char, two ships are ajacent - important note, if out of bounds, charAt returns ' '
				if (charAt(i) != ch && (charAt(i) != ' ')) { valid = false; }
				i.col -= 2;
				if (charAt(i) != ch && (charAt(i) != ' ')) { valid = false; }
				i.col++;

				i.row++;			//repeat the process for row
				if (charAt(i) != ch && (charAt(i) != ' ')) { valid = false; }
				i.row -= 2;
				if (charAt(i) != ch && (charAt(i) != ' ')) { valid = false; }
				i.row++;

				i.depth++;			//repeat the process for depth
				if (charAt(i) != ch && (charAt(i) != ' ')) { valid = false; }
				i.depth -= 2;
				if (charAt(i) != ch && (charAt(i) != ' ')) { valid = false; }
				i.depth++;
			}
		}
	}
}
vector<Ship> Board::scan() {
	vector<Ship> result;
	Coordinate i = Coordinate(0, 0, 0);
	for (i.row = 1; i.row <= rows(); i.row++) {
		for (i.col = 1; i.col <= cols(); i.col++) {
			for (i.depth = 1; i.depth <= depth(); i.depth++) {
				if (getCounter(i) != 0) {
					Ship shipAddr = Ship('z', DIRECTION::X);
					scanShip(i, shipAddr);
					if (shipAddr.getType() != 'z') {
						result.push_back(shipAddr); //would work with scanShip as well as addShip 	 //if (islower(charAt(i))) {counterSideA++; }		 //else  {counterSideB++; 
					} else { //we have a defective ship and need to end the game
						valid = false;
					}
				}
			}
		}
	}
	return result;
}

bool Board::isValid() {
	return valid;
}




Board::Board(string filename) {
	string line;
	Coordinate loc = Coordinate(0, 0, 0);

	//check cwd
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	//cout << "Working Dir: " << string(buffer).substr(0, pos) << endl;      // PRINT CWD 
	ifstream boardFile(filename);
	if (boardFile.is_open()) {
		getline(boardFile, line);
		if (sscanf_s(line.c_str(), "%dx%dx%d", &loc.col, &loc.row, &loc.depth) != 3) {
			valid = false;
			return;
		}
		_rows = loc.row;
		_cols = loc.col;
		_depth = loc.depth;

		board = new Cell**[_rows];
		for (int i = 0; i < _rows; i++) {
			board[i] = new Cell*[_cols];
			for (int j = 0; j < _cols; j++) {
				board[i][j] = new Cell[_depth];
			}
		}
		loc.depth = 1;
		bool infile = true;
		infile = true;
		for (loc.depth=1; loc.depth <= depth(); loc.depth++) {
			if (infile) {
				getline(boardFile, line);
				if (boardFile.eof())
					infile = false;
			}
			for (loc.row = 1; loc.row <= rows(); loc.row++) {
				if (infile) {
					getline(boardFile, line);
					if (boardFile.eof())
						infile = false;
				}
				else {
					line = "";
					for (loc.col = 1; loc.col <= cols(); loc.col++) {
						line.push_back(' ');
					}
				}
				//cout << "[" << line << "]" << endl;
				for (loc.col = 1; loc.col <= cols(); loc.col++) {
					//check for input vaildity
					switch (line[loc.col - 1]) {
					case 'm':
					case 'M':
					case 'D':
					case 'd':
					case 'p':
					case 'P':
					case 'b':
					case 'B':
						break;
					default:
						line[loc.col - 1] = ' ';
						break;
					}
					initSet(loc, line[loc.col - 1]);
				}
			}
		}
		pair<int,int> res = scan_Board();
	}
	else {
		printf("Unable to open (file is close) %s\n ", filename.c_str());
		return;
	}
}


void Board::init(BoardData const &origin) {
	_rows = origin.rows();
	_cols = origin.cols();
	_depth = origin.depth();
	board = new Cell**[_rows];
	for (int i = 0; i < _rows; i++) {
		board[i] = new Cell*[_cols];
		for (int j = 0; j < _cols; j++) {
			board[i][j] = new Cell[_depth];
		}
	}
	Coordinate loc = Coordinate(0, 0, 0);
	for (loc.depth = 1; loc.depth <= depth(); loc.depth++) {
		for (loc.row = 1; loc.row <= rows(); loc.row++) {
			for (loc.col = 1; loc.col <= cols(); loc.col++) {
				initSet(loc, origin.charAt(loc));
			}
		}
	}
	scan_Board();
}

Coordinate Board::getSize() const {
	return Coordinate(rows(), cols(), depth());
}

Board::Board(Coordinate size) {
	_rows = size.row;
	_cols = size.col;
	_depth = size.depth;
	board = new Cell**[_rows];
	for (int i = 0; i < _rows; i++) {
		board[i] = new Cell*[_cols];
		for (int j = 0; j < _cols; j++) {
			board[i][j] = new Cell[_depth];
		}
	}	
}

void Board::operator=(Board const &x) {
	_rows = x.rows();
	_cols = x.cols();
	_depth = x.depth();
	board = new Cell**[_rows];
	for (int i = 0; i < _rows; i++) {
		board[i] = new Cell*[_cols];
		for (int j = 0; j < _cols; j++) {
			board[i][j] = new Cell[_depth];
		}
	}
	ships = x.ships;
	for (int i = 0; i < rows(); i++)
		for (int j = 0; j < cols(); j++)
			for (int k = 0; k < depth(); k++)
				board[i][j][k] = x.board[i][j][k];

}
//returns a partial board, that has no "illegal" information for players to use
 Board Board::getSidedBoard(bool sideA) const {
	 Coordinate i = getSize();
	 Board sidedBoard = Board(i);
	//init to clear board
	print();
	for (i.row = 1; i.row <= _rows; ++i.row) {
		for (i.col = 1; i.col <= _cols; ++i.col) {
			for (i.depth = 1; i.depth <= _depth; ++i.depth) {
				if (IsCharUpperA(charAt(i)) != 0 && sideA) {
					sidedBoard.set(i, charAt(i));
				}
				else if (IsCharUpperA(charAt(i)) == 0 && (sideA == false)) {
					sidedBoard.set(i, charAt(i));
				}
				else {
					sidedBoard.set(i, ' ');
				}
			}
		}
	}
	return sidedBoard;
}
 //determine wether or not the game is over
bool Board::gameOver(bool sideA) {
	for (auto ship : ships) {
		if ((ship.isSideA() == sideA) && ship.checkAlive())
			return false;
	}
	return true;
}




// function that modifies the char value of a given coordinate on board --remember it starts from 1!!!
void Board::set(Coordinate loc, char type) {
	board[loc.row - 1][loc.col - 1][loc.depth - 1].value = type;
}
//this it used to initialize values used to calculate heat map results, which will, in turn, help us attack in the proper areas
void Board::initSet(Coordinate loc, char type) {
	cellAt(loc)->heatValues[XL] = loc.row;
	cellAt(loc)->heatValues[XH] = rows() - loc.row + 1;
	cellAt(loc)->heatValues[YL] = loc.col;
	cellAt(loc)->heatValues[YH] = cols() - loc.col + 1;
	cellAt(loc)->heatValues[ZL] = loc.depth;
	cellAt(loc)->heatValues[ZH] = depth() - loc.depth + 1;
	cellAt(loc)->value = type;
}

/*function checks if there is ship there
return values:
0 - no ship there
1 - A's ship there, alive and well
3 - A's ship there, dead
2 - B's ship there, alive and well
4 - B's ship there, dead
*/
int Board::isShipThere(Coordinate location) {
	for (auto ship : ships) {
		if (ship.checkLocation(location)) {
			if (ship.isSideA() && ship.checkAlive())
				return 1;
			if (ship.isSideA() && !ship.checkAlive())
				return 3;
			if (!ship.isSideA() && ship.checkAlive())
				return 2;
			if (!ship.isSideA() && !ship.checkAlive())
				return 4;
		}
	}
	return 0;
}

void Board::print(bool heat) {//prints the board on the screen (bool is used to determine wether the print should be of the heat map values used to attack, or of the char array that is board.
	Coordinate loc = Coordinate(0, 0, 0);
	for (loc.depth = 1; loc.depth <= depth(); loc.depth++) {
		printf("depth: %d\n", loc.depth);
		for (loc.row = 0; loc.row <= rows(); loc.row++) { //print line marks also
			for (loc.col = 1; loc.col <= cols(); loc.col++)
				printf("%c%c%c%c%c%c", char(178), char(178), char(178), char(178), char(178), char(178));
			cout << char(178) << endl << char(178);
			if (heat) {
				for (loc.col = 1; loc.col <= cols(); loc.col++) {
					if (loc.row == 0)
						printf(" %3d %c", loc.col, char(178));
					else if (loc.row <= rows())
						printf(" %3d %c", cellAt(loc)->score(), char(178));
				}
			}
			else {
				for (loc.col = 1; loc.col <= cols(); loc.col++) {
					if (loc.row == 0)
						printf(" %3d %c", loc.col, char(178));
					else if (loc.row <= rows())
						printf("  %c  %c", charAt(loc), char(178));
				}
			}
			cout << endl;
		}
		cout <<endl;
	}
}

void Board::print() const {//prints the board on the screen (bool is used to determine wether the print should be of the heat map values used to attack, or of the char array that is board.
	return;
	Coordinate loc = Coordinate(0, 0, 0);
	for (loc.depth = 1; loc.depth <= depth(); loc.depth++) {
		printf("depth: %d\n", loc.depth);
		for (loc.row = 0; loc.row <= rows(); loc.row++) { //print line marks also
			for (loc.col = 1; loc.col <= cols(); loc.col++)
				printf("%c%c%c%c%c%c", char(178), char(178), char(178), char(178), char(178), char(178));
			cout << char(178) << endl << char(178);
			for (loc.col = 1; loc.col <= cols(); loc.col++) {
				if (loc.row == 0)
					printf(" %3d %c", loc.col, char(178));
				else if (loc.row <= rows())
					printf("  %c  %c", charAt(loc), char(178));
			}
			cout << endl;
		}
		cout << endl;
	}
	//for (auto &s : ships)
	//s.print();
}

char Board::charAt(Coordinate c) const { //return ' ' for non exist coords
	if (!(c.row > 0 && c.row <= rows())) {
		return ' ';
	}
	if (!(c.col > 0 && c.col <= cols())) {
		return ' ';
	}
	if (!(c.depth > 0 && c.depth <= depth())) {
		return ' ';
	}
	return board[c.row - 1][c.col - 1][c.depth - 1].value;
}


// ================ [Assisting Methods] ====================//

int pathExist(string pathname) {
	struct stat info;
	if (stat(pathname.c_str(), &info) != 0)
		return ERR_PATH_DOESNT_EXIST;
	else if (info.st_mode & S_IFDIR)  // S_ISDIR() doesn't exist on my windows 
		return 0;
	else
		return ERR_PATH_NOT_FOLDER;
}

vector<string> get_all_files_names_within_folder(string folder, string fileType) {
	vector<string> names;
	string search_path = folder + "/*." + fileType;
	WIN32_FIND_DATAA fd;
	HANDLE hFind = ::FindFirstFileA(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(folder + "\\" + fd.cFileName);
			}
		} while (::FindNextFileA(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

//Note – when we enter this function - the board has been cleared of non-ship chars (=,%,etc), and we are at the shallowest(none deep), upper, left block of the ship we encounter.
// this function contains the logic of verifying ship legallity. it uses a variety of conditions to assert correct shape and size. if the ship is legal we add it to the board(addShip), 
// and otherwise we remove it(burnShip)
void Board::scanShip(Coordinate cor, Ship& fillShip) { //
	Coordinate i = Coordinate(cor.row, cor.col, cor.depth);
	char ch = charAt(cor);
	int counter = getCounter(i);
	bool flag_col = check_col(i, charAt(i));
	bool flag_row = check_row(i, charAt(i));
	bool flag_depth = check_depth(i, charAt(i));
	if (flag_row + flag_col + flag_depth > 1) {
		burnShip(cor, charAt(cor));
	} else if (flag_row + flag_col + flag_depth == 0){
		if (counter == 1) { //B ship case//
			fillShip = addShip(cor);
			return;
		}
		burnShip(cor, charAt(cor));
		return;
	} else {//this case : ship has a single direction and there are more cells 
		while (charAt(i) == ch) {
			counter--;
			if (!CoordOnBoard(i)) { 
				break; 
			} 
			if (flag_col && (check_row(i, charAt(i)) || check_depth(i, charAt(i)))) {
				burnShip(cor, charAt(cor));
				return;
			} else if (flag_row && (check_col(i, charAt(i)) || check_depth(i, charAt(i)))) {
				burnShip(cor, charAt(cor));
				return;
			} else if (flag_depth && (check_row(i, charAt(i)) || check_col(i, charAt(i)))) {
				burnShip(cor, charAt(cor));
				return;
			}
			i.col = i.col + flag_col; i.row = i.row + flag_row; i.depth = i.depth + flag_depth;//advance in right direction	
		}
		if (counter == 0) {
			fillShip = addShip(cor); //the correct length, with no shape deformeties found
			return;
		} else { 
			burnShip(cor, charAt(cor)); 
			return; 
		}
	}
}



void Board::burnShip(Coordinate i, char ch) {//upon finding an illegal ship, we make it disappear from the face of the earth (board)
											 //works recursively and changes the value, so as not to start a loop
	valid = false;
	if (!CoordOnBoard(i)) { return; }
	if (charAt(i) == ch) { //remove ship so we don't find it when looking for ajancencies. recursively move to neighbors (wierd shaped ships are hard to mold)
		set(i, ' ');
		i.col += 1;
		burnShip(i, ch);
		i.col -= 2;
		burnShip(i, ch);
		i.col += 1;

		i.row += 1;
		burnShip(i, ch);
		i.row -= 2;
		burnShip(i, ch);
		i.row += 1;

		i.depth += 1;
		burnShip(i, ch);
		i.depth -= 2;
		burnShip(i, ch);
		i.depth += 1;
	}
	return;
}


int Board::getCounter(Coordinate i){// this function converts the characters of a ship to it's expected length
	if ((charAt(i) == 'b') || (charAt(i) == 'B')) {
		return 1;
	}
	if ((charAt(i) == 'p') || (charAt(i) == 'P')) {
		return 2;
	}
	if ((charAt(i) == 'm') || (charAt(i) == 'M')) {
		return 3;
	}
	if ((charAt(i) == 'd') || (charAt(i) == 'D')) {
		return 4;
	}
	return 0;
}


Ship Board::addShip(Coordinate i) {//this function is called after verifying a ship is legal. it constructs the ship and adds it to the board's ship vector
	vector<Coordinate> result;
	int counter = getCounter(i);
	bool col_flag, row_flag, depth_flag;
	col_flag = check_col(i, charAt(i));
	row_flag = check_row(i, charAt(i));
	depth_flag = check_depth(i, charAt(i));
	Coordinate temp = Coordinate(i.row, i.col, i.depth);
	for (int j = 0; j < counter; j++) {
		result.push_back(temp);
		set(temp, static_cast<char>(charAt(temp) + 1));//put the next char so we don't find this ship from the middle again
		temp.row += row_flag; temp.col += col_flag; temp.depth += depth_flag;
	}
	DIRECTION direction = X;
	if (row_flag) {
		direction = X;
	}
	if (col_flag) {
		direction = Y;
	}
	if (depth_flag) {
		direction = Z;
	}
	Ship shipToAdd(static_cast<char>(charAt(i)-1), direction);
	shipToAdd.putInPlace(result);
	return shipToAdd;
}


bool Board::check_depth(Coordinate cor, char ch) { //return true iff the neighbors on the depth dimention have the same char-are part of the same ship
	Coordinate i = Coordinate(cor.row, cor.col, cor.depth-1);
	bool flag = false;
	flag = flag || (charAt(i) == ch);
	i.depth += 2;
	flag = flag || (charAt(i) == ch);
	return flag;
}

bool Board::check_row(Coordinate cor, char ch) {//return true iff the neighbors on the row dimention have the same char-are part of the same ship
	Coordinate i = Coordinate(cor.row-1, cor.col, cor.depth);
	bool flag = false;
	flag = flag || (charAt(i) == ch);
	i.row += 2;
	flag = flag || (charAt(i) == ch);
	return flag;
}
bool Board::check_col(Coordinate cor, char ch) {//return true iff the neighbors on the col dimention have the same char-are part of the same ship
	Coordinate i = Coordinate(cor.row, cor.col-1, cor.depth);
	bool flag = false;
	flag = flag || (charAt(i) == ch);
	i.col += 2;
	flag = flag || (charAt(i) == ch);
	return flag;
}



// function returns true if coord is a legitimate position on board
bool Board::CoordOnBoard(Coordinate coord) {
	if (!(coord.row > 0 && coord.row <= rows()))
		return false;
	if (!(coord.col > 0 && coord.col <= cols()))
		return false;
	if (!(coord.depth > 0 && coord.depth <= depth()))
		return false;
	return true;
}

Cell* Board::cellAt(Coordinate c) {
	if (CoordOnBoard(c))
		return &board[c.row - 1][c.col - 1][c.depth - 1];
	return NULL;
}

void Board::kaboom(Coordinate loc) { //"bombard" a cell, making it impassible for other cells, and changing the heat map values according to the impass it creates
	Coordinate i = Coordinate(1, 1, 1);

	i.col = loc.col;
	i.depth = loc.depth;
	for (i.row=1; i.row <= rows(); i.row++) {
		if (i.row < loc.row)
			cellAt(i)->kaboom(XL,abs(i.row - loc.row));
		else
			cellAt(i)->kaboom(XH, abs(i.row - loc.row));
	}
	
	i.row = loc.row;
	i.depth = loc.depth;
	for (i.col=1; i.col <= cols(); i.col++) {
		if (i.col < loc.col)
			cellAt(i)->kaboom(YL, abs(i.col - loc.col));
		else
			cellAt(i)->kaboom(YH, abs(i.col - loc.col));
	}

	i.col = loc.col;
	i.row = loc.row;
	for (i.depth=1; i.depth <= depth(); i.depth++) {
		if (i.depth < loc.depth)
			cellAt(i)->kaboom(ZL, abs(i.depth - loc.depth));
		else
			cellAt(i)->kaboom(ZH, abs(i.depth - loc.depth));
	}

	cellAt(loc)->kaboom(XL,0);
	cellAt(loc)->kaboom(YL,0);
	cellAt(loc)->kaboom(ZL,0);
}

void Board::kaboom(Ship ship) {//this function updates an entire ship and it's immediate serrounding to be impassible, like the previous kaboom but for the whole ship.
	for (auto &m : ship.location()) {
		Coordinate temp = m.first;
		kaboom(m.first);
		temp.row--;
		if (CoordOnBoard(temp)) 
			kaboom(temp);
		temp.row += 2;
		if (CoordOnBoard(temp))
			kaboom(temp);
		temp.row--;
		temp.col--;
		if (CoordOnBoard(temp))
			kaboom(temp);
		temp.col += 2;
		if (CoordOnBoard(temp))
			kaboom(temp);
		temp.col--;
		temp.depth--;
		if (CoordOnBoard(temp))
			kaboom(temp);
		temp.depth += 2;
		if (CoordOnBoard(temp))
			kaboom(temp);
		temp.depth--;
	}
}

void Board::kaboom(vector<Coordinate> kaboomboom) {//much like the other two kabooms, this function "bombards" the enemy ship, the result of an attack indicates it's destruction
	for (auto &m : kaboomboom) {
		Coordinate temp = m;
		temp.row--;
		if (CoordOnBoard(temp))
			kaboom(temp);
		temp.row += 2;
		if (CoordOnBoard(temp))
			kaboom(temp);
		temp.row--;
		temp.col--;
		if (CoordOnBoard(temp))
			kaboom(temp);
		temp.col += 2;
		if (CoordOnBoard(temp))
			kaboom(temp);
		temp.col--;
		temp.depth--;
		if (CoordOnBoard(temp))
			kaboom(temp);
		temp.depth += 2;
		if (CoordOnBoard(temp))
			kaboom(temp);
		temp.depth--;
	}
}

