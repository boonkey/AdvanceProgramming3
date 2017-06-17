#include "Board.h"

/*Board::Board(Coordinate size) {
	_rows = size.row;
	_cols = size.col;
	_depth = size.depth;

	board = new char**[_rows];
	for (int i = 0; i < _rows; ++i) {
		board[i] = new char*[_cols];
		for (int j = 0; j < _depth; j++) {
			board[i][j] = new char[_depth];
		}
	}
	//init to clear board
	for (int i = 0; i < _rows; ++i) {
		for (int j = 0; j < _cols; ++j) {
			for (int k = 0; k < _depth; ++k) {
				board[i][j][k] = ' ';
			}
		}
	}
}*/

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
		cout << line << endl;
		sscanf_s(line.c_str(), "%dx%dx%d", &loc.col, &loc.row, &loc.depth);
		PRINT(loc) << endl;
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
		getline(boardFile, line);
		while (loc.depth <= _depth) {
			loc.row = 1;
			while (getline(boardFile, line) && loc.row <= _rows) {
				for (loc.col = 1; loc.col <= _cols; loc.col++) {
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
				loc.row++;
			}
			
			loc.row = 1;
			loc.depth++;
		}
		//print();
	}
	else {
		printf("Unable to open (file is close) %s\n ", filename.c_str());
		return;
	}
}

Board::Board(Board& origin) {
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
	board[loc.row - 1][loc.col - 1][loc.depth - 1].heatValues[XL] = loc.row;
	board[loc.row - 1][loc.col - 1][loc.depth - 1].heatValues[XH] = rows() - loc.row + 1;
	board[loc.row - 1][loc.col - 1][loc.depth - 1].heatValues[YL] = loc.col;
	board[loc.row - 1][loc.col - 1][loc.depth - 1].heatValues[YH] = cols() - loc.col + 1;
	board[loc.row - 1][loc.col - 1][loc.depth - 1].heatValues[ZL] = loc.depth;
	board[loc.row - 1][loc.col - 1][loc.depth - 1].heatValues[ZH] = depth() - loc.depth + 1;
	board[loc.row - 1][loc.col - 1][loc.depth - 1].value = type;
}

void Board::print() {
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
					printf(" %3d %c", cellAt(loc)->score(), char(178));
			}
			cout << endl;
		}
		cout <<endl;
	}
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





// function returns true if coord is a legitimate position on board
bool Board::CoordOnBoard(Coordinate coord) {
	if (!(coord.row > 0 && coord.row < rows()))
		return false;
	if (!(coord.col > 0 && coord.col < cols()))
		return false;
	if (!(coord.depth > 0 && coord.depth < depth()))
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

