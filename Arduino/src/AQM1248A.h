#pragma once

#include <arduino.h>
#include "TinyStickDef.h"

//
// AQM1248A class
//
class AQM1248A {
public:
	static const uint8_t WIDTH = 128;
	static const uint8_t HEIGHT = 48;
	static const uint8_t PAGES = 6;

	void begin(uint8_t pin_RS, uint8_t pin_CS, uint8_t pin_BL);

	void backLight(bool on) {
		digitalWrite(_pin_BL, on);
		_backLight = on;
	}
	bool isBackLight() { return _backLight; }

	void clear();
	void flush(uint8_t offset = 0);
	void drawPixel(int16_t x, int16_t y, bool set = 1);

private:
	uint8_t _pin_RS;
	uint8_t _pin_CS;
	uint8_t _pin_BL;

	uint8_t _vram[PAGES][WIDTH];

	void cmd(uint8_t cmd);
	void cmds(const uint8_t cmds[], uint8_t len);
	void data(uint8_t data);

protected:
	bool _backLight = false;

#ifdef LCD_VERTICAL
	bool _vertical = false;
	uint8_t _width = WIDTH;
	uint8_t _height = HEIGHT;
#endif
};

#ifndef LCD_VERTICAL
#define _width WIDTH
#define _height HEIGHT
#endif

