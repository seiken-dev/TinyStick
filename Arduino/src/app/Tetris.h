#pragma once

#include "TinyApp.h"

class Tetris : public TinyApp {
public:
	virtual const char* getAppName() { return "TE*RIS"; }
	virtual void update();
	virtual void setActive(bool isActive);
	void start();

private:
	int8_t minoX, minoY;
	uint8_t minoRotate;
	uint8_t minoType;

	bool isFalling;
	bool isThrowIn;
	uint8_t flashCount;
	unsigned long prevTick;

	void throwIn();
	void clearMino();
	void landingMino();
	int8_t putMino();
	bool canMoveOrRotate(int8_t dx, int8_t rotate);
	bool findLinedUp();
	void eraseLines();
	void flash();
	void drawCell();
	void gameOver();
};