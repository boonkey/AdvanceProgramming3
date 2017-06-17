#include "Cell.h"


int Cell::score() {
	heatValues[XM] = abs(heatValues[XH] + heatValues[XL]) - abs(heatValues[XH] - heatValues[XL]);
	heatValues[YM] = abs(heatValues[YH] + heatValues[YL]) - abs(heatValues[YH] - heatValues[YL]);
	heatValues[ZM] = abs(heatValues[ZH] + heatValues[ZL]) - abs(heatValues[ZH] - heatValues[ZL]);
	return heatValues[XM] * heatValues[YM] * heatValues[ZM];
}

void Cell::kaboom(HEATMODIFIER i,int score) {
	heatValues[i] = min(heatValues[i],score);
}

void Cell::print() {
	score();
	cout << "Value: " << value << " Heatvlues: | ";
	for (auto &h : heatValues) {
		cout << h << " | ";
	}
	cout << " Heat: " << score() << endl;
}