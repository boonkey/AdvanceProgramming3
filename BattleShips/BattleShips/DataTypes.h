#pragma once

#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <utility> // for std::pair
#include <vector>
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
using namespace std;

#define BOARD_SIZE 10
#define PRINT(loc) cout << "< " << loc.row << " , " << loc.col << " , " << loc.depth << "> " << endl

//================== Error Code Definition ===========
#define ERR_WRONG_NUM_OF_ARGS  -1
#define ERR_TOO_MUCH_FILES     -2
#define ERR_PATH_DOESNT_EXIST  -3
#define ERR_PATH_NOT_FOLDER    -4
#define ERR_NUM_OF_ALGO		   -5
#define ERR_NUM_OF_ATTACKB     -6
#define ERR_NUM_OF_BOARDS      -7
#define ERR_MISSING_ATTACKA    -8
#define ERR_MISSING_ATTACKB    -9
#define ERR_MISSING_SBOARD     -10
#define ERR_LOADING_BOARD      -11
#define ERR_BAD_BOARD          -12
#define ERR_UNREACHABLE        -13

struct Configuration{
	string workingDirectory = ".";
	string attackA, attackB, pathBoard;
	int delay = 600;
	bool quiet = false;
	int scoreA = 0;
	int scoreB = 0;
};

extern struct Configuration config;

enum DIRECTION {
	X = 0, // Horizontal
	Y = 1, // Vectral
	Z = 2  // Inwards
};

enum COLOR
{
	BLACK = 0,
	DARK_BLUE = 1,
	DARK_GREEN = 2,
	DARK_AQUA, DARK_CYAN = 3,
	DARK_RED = 4,
	DARK_PURPLE = 5, DARK_PINK = 5, DARK_MAGENTA = 5,
	DARK_YELLOW = 6,
	DARK_WHITE = 7,
	GRAY = 8,
	BLUE = 9,
	GREEN = 10,
	AQUA = 11, CYAN = 11,
	RED = 12,
	PURPLE = 13, PINK = 13, MAGENTA = 13,
	YELLOW = 14,
	WHITE = 15
};