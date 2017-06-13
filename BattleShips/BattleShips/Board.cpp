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
		PRINT(loc);
		_rows = loc.row;
		_cols = loc.col;
		_depth = loc.depth;

		board = new char**[_rows];
		for (int i = 0; i < _rows; i++) {
			board[i] = new char*[_cols];
			for (int j = 0; j < _cols; j++) {
				board[i][j] = new char[_depth];
			}
		}
		cout << "Starting: " << endl;
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
					set(loc, line[loc.col - 1]);
				}
				loc.row++;
			}
			
			loc.row = 1;
			loc.depth++;
		}
		print();
	}
	else {
		printf("Unable to open (file is close) %s\n ", filename.c_str());
		return;
	}
}

const char*** Board::getSidedBoard(bool sideA) {
	char*** sidedBoard = new char**[_rows];
	for (int i = 0; i < _rows; i++) {
		sidedBoard[i] = new char*[_cols];
		for (int j = 0; j < _cols; j++) {
			sidedBoard[i][j] = new char[_depth];
		}
	}
	//init to clear board
	Coordinate i = Coordinate(0,0,0);
	for (i.row = 0; i.row < _rows; ++i.row) {
		for (i.col = 0; i.col < _cols; ++i.col) {
			for (i.depth = 0; i.depth < _depth; ++i.depth) {
				if (IsCharUpperA(charAt(i)) != 0 && sideA)
					sidedBoard[i.row][i.col][i.depth] = charAt(i);
				else if (IsCharLowerA(charAt(i)) != 0 && !sideA)
					sidedBoard[i.row][i.col][i.depth] = charAt(i);
				else
					sidedBoard[i.row][i.col][i.depth] = ' ';
			}
		}
	}
	return const_cast<const char ***> (sidedBoard);
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
	board[loc.row - 1][loc.col - 1][loc.depth - 1] = type;
}

void Board::print() {
	Coordinate loc = Coordinate(0, 0, 0);
	for (loc.depth = 1; loc.depth <= depth(); loc.depth++) {
		for (loc.row = 1; loc.row <= rows(); loc.row++) {
			for (loc.col = 1; loc.col <= cols(); loc.col++) {
				cout << charAt(loc) << char(176);
			}
			cout << endl;
			for (loc.col = 1; loc.col <= cols(); loc.col++) {
				cout << char(176) << char(176);
			}
			cout << endl;
		}
		cout << endl;
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
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFileA(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}












int main() {
	string path = "E:\\AdvancedPrograming\\BattleShips\\test_files";
	string boardpath = get_all_files_names_within_folder(path, "sboard")[2];
	Board board = Board(path + "\\" + boardpath);

	system("pause");
}