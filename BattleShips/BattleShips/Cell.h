#pragma once
#include "DataTypes.h"


class Cell {
public:
	char value;
	int heatValues[9] = { 0,0,0,0,0,0,0,0,0 };
	int score();
	void kaboom(HEATMODIFIER i,int score);
	void print();

};