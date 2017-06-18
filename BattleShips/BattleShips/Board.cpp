#include "Board.h"

pair<int, int> Board::scan_Board() {
	cleanBoard();
	//print();
	//system("pause");
	int shipCounterA = 0;
	int shipCounterB = 0;
	ships = scan();
	restoreBoard();
	checkAjancencies();
	for (auto item : ships) {
		//item.print();
		if (item.isSideA()) {
			shipCounterA++;
		}
		else {
			shipCounterB++;
		}
	}
	pair<int,int> result = make_pair(shipCounterA, shipCounterB);
	return result;
	//TODO - ADD prints for incorrect Board alligment
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
	//	int counterSideA;
	//	int counterSideB;
	Coordinate i = Coordinate(0, 0, 0);
	//cout << "Scan Started" << endl;
	for (i.row = 1; i.row <= rows(); i.row++) {
		for (i.col = 1; i.col <= cols(); i.col++) {
			for (i.depth = 1; i.depth <= depth(); i.depth++) {
				//PRINT(i) << " Scanning: " << charAt(i) << endl;
				if (getCounter(i) != 0) {
					//cout << endl << "Found a Ship: " << charAt(i) << endl;
				//	PRINT(i) << endl;
					Ship shipAddr = Ship('z', DIRECTION::X);
					scanShip(i, shipAddr);
					if (shipAddr.getType() != 'z') {
						result.push_back(shipAddr); //would work with scanShip as well as addShip 	 //if (islower(charAt(i))) {counterSideA++; }		 //else  {counterSideB++; 
					} else { //we have a defective ship and need to end the game
						cout << "I have identified an illegal ship without blowing up - delete this print" << endl;
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
		//cout << line << endl;
		if (sscanf_s(line.c_str(), "%dx%dx%d", &loc.col, &loc.row, &loc.depth) != 3) {
			valid = false;
			return;
		}
		//PRINT(loc) << endl;
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
		//print();
		
		pair<int,int> res = scan_Board();
		printf("Board Anali [%s]: found %d ships for A, and %d ships for B\n", filename.c_str(), res.first, res.second);
		//print();
		//system("pause");
	}
	else {
		printf("Unable to open (file is close) %s\n ", filename.c_str());
		return;
	}
}

Board::Board(Board const &origin) {
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

 Board Board::getSidedBoard(bool sideA) {
	Board sidedBoard = Board(*this);
	//init to clear board
	Coordinate i = Coordinate(0,0,0);
	for (i.row = 1; i.row < _rows; ++i.row) {
		for (i.col = 1; i.col < _cols; ++i.col) {
			for (i.depth = 1; i.depth < _depth; ++i.depth) {
				if (IsCharUpperA(charAt(i)) != 0 && sideA)
					sidedBoard.set(i, charAt(i));
				else if (IsCharLowerA(charAt(i)) != 0 && !sideA)
					sidedBoard.set(i, charAt(i));
				else
					sidedBoard.set(i, ' ');
			}
		}
	}
	
	return sidedBoard;
}

bool Board::gameOver(bool sideA) {
	for (auto ship : ships) {
		if ((ship.isSideA() == sideA) && ship.checkAlive())
			return false;
	}
	return true;
}




// set works from 1!!!!!!!!
void Board::set(Coordinate loc, char type) {
	board[loc.row - 1][loc.col - 1][loc.depth - 1].value = type;
}

void Board::initSet(Coordinate loc, char type) {
	cellAt(loc)->heatValues[XL] = loc.row;
	cellAt(loc)->heatValues[XH] = rows() - loc.row + 1;
	cellAt(loc)->heatValues[YL] = loc.col;
	cellAt(loc)->heatValues[YH] = cols() - loc.col + 1;
	cellAt(loc)->heatValues[ZL] = loc.depth;
	cellAt(loc)->heatValues[ZH] = depth() - loc.depth + 1;
	cellAt(loc)->value = type;
}

void Board::print(bool heat) {
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

//Note – when we enter this function - the board has been cleared of none - ship chars, and we are at the shallowest(none deep), upper, left block of the ship.
//also – getchar(out of board index) = special_reserved_val
void Board::scanShip(Coordinate cor, Ship& fillShip) {
	Coordinate i = Coordinate(cor.row, cor.col, cor.depth);
	char ch = charAt(cor);
	int counter = getCounter(i);
	bool flag_col = check_col(i, charAt(i));
	bool flag_row = check_row(i, charAt(i));
	bool flag_depth = check_depth(i, charAt(i));
	if (flag_row + flag_col + flag_depth > 1) {
		burnShip(cor, charAt(cor));
	} else if (flag_row + flag_col + flag_depth == 0){
		if (counter == 1) { //B ship case
																		//{code to make a ship and than return it, with it's whole vector of body is i}
			fillShip = addShip(cor);
			return;
		}
		burnShip(cor, charAt(cor));
		return;
		//print("make sure you change me when you know syntax");
	} else {//this case : ship has a single direction and there is more ship Note- should belong to else if.
		//i.col = i.col + flag_col; i.row = i.row + flag_row; i.depth = i.depth + flag_depth;//advance in right direction	
		while (charAt(i) == ch && ((flag_col | flag_row | flag_depth)|1)) {
			//cout << "Checking @: ";
			//PRINT(i) << " where there is: " << charAt(i) << endl;
			counter--;
			if (!CoordOnBoard(i)) { 
			//	PRINT(i);
				//cout << "board has failed me" << endl;
				break; 
			}//we need this 
			if (flag_col && (check_row(i, charAt(i)) || check_depth(i, charAt(i)))) {
				burnShip(cor, charAt(cor));
			//	cout << "col has failed me" << endl;
				return;
			} else if (flag_row && (check_col(i, charAt(i)) || check_depth(i, charAt(i)))) {
				burnShip(cor, charAt(cor));
			//	cout << "row has failed me" << endl;
				return;
			} else if (flag_depth && (check_row(i, charAt(i)) || check_col(i, charAt(i)))) {
				burnShip(cor, charAt(cor));
				//cout << "depth has failed me" << endl;
				return;
			}
			i.col = i.col + flag_col; i.row = i.row + flag_row; i.depth = i.depth + flag_depth;//advance in right direction	
		}
		if (counter == 0) {
			fillShip = addShip(cor); //ship len is just right and we didn't find any evidence it's illegal
			return;
		} else { 
			burnShip(cor, charAt(cor)); 
		//	cout << "counter has failed me: " << counter << endl;
			return; 
		}//too short or too long
	}
}



void Board::burnShip(Coordinate i, char ch) {
	valid = false;
	//if you guys wanna do a logger, than we should log invalid size/shape in scanship when calling this func(shape for inside the while loop and size for counter fail)
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


int Board::getCounter(Coordinate i){
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


Ship Board::addShip(Coordinate i) {
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
	//shipToAdd.print();
	return shipToAdd;
}


bool Board::check_depth(Coordinate cor, char ch) { //return true iff the neighbors on the depth dimention have the same char
	Coordinate i = Coordinate(cor.row, cor.col, cor.depth-1);
	bool flag = false;
	flag = flag || (charAt(i) == ch);
	i.depth += 2;
	flag = flag || (charAt(i) == ch);
	//cout << "depth: " << flag << endl;
	return flag;
}

bool Board::check_row(Coordinate cor, char ch) {//return true iff the neighbors on the row dimention have the same char
	Coordinate i = Coordinate(cor.row-1, cor.col, cor.depth);
	bool flag = false;
	flag = flag || (charAt(i) == ch);
	i.row += 2;
	flag = flag || (charAt(i) == ch);
	//cout << "row: " << flag << endl;
	return flag;
}
bool Board::check_col(Coordinate cor, char ch) {//return true iff the neighbors on the col dimention have the same char
	Coordinate i = Coordinate(cor.row, cor.col-1, cor.depth);
	bool flag = false;
	flag = flag || (charAt(i) == ch);
	i.col += 2;
	flag = flag || (charAt(i) == ch);
	//cout << "col: " << flag << endl;
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
	return &board[c.row - 1][c.col - 1][c.depth - 1];
}

void Board::kaboom(Coordinate loc) {
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

void Board::kaboom(Ship ship) {
	for (auto &m : ship.location()) {
		Coordinate temp = m.first;
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

