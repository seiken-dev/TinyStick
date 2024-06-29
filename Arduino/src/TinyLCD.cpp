#include <Arduino.h>
#include <SPI.h>
#include "TinyLCD.h"

//
// TinyLCD class
//
// https://github.com/SWITCHSCIENCE/samplecodes/tree/master/AQM1248A_breakout/Arduino/AQM1248A_lib

TinyLCD LCD;

#ifndef LCD_VERTICAL
#define _width WIDTH
#define _height HEIGHT
#endif

#define CMD_DELAY 0xFF

const static uint8_t initCmds[] = {0xAE, 0xA0, 0xC8, 0xA3, 0x2C, CMD_DELAY, 0x2E, CMD_DELAY, 0x2F, 0x23, 0x81, 0x1C, 0xA4, 0x40, 0xA6, 0xAF };

void TinyLCD::begin(uint8_t pin_RS, uint8_t pin_CS, uint8_t pin_BL) {
	_pin_RS = pin_RS;
	_pin_CS = pin_CS;
	_pin_BL = pin_BL;

	pinMode(_pin_RS, OUTPUT);
	pinMode(_pin_CS, OUTPUT);
	pinMode(_pin_BL, OUTPUT);
	digitalWrite(_pin_CS, HIGH);

	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV2);
	SPI.setDataMode(SPI_MODE3);

	cmds(initCmds, sizeof(initCmds));

	clear();
}

void TinyLCD::cmd(uint8_t cmd) {
	digitalWrite(_pin_RS, LOW);
	digitalWrite(_pin_CS, LOW);
	SPI.transfer(cmd);
	digitalWrite(_pin_CS, HIGH);
}

void TinyLCD::cmds(const uint8_t cmds[], uint8_t len) {
	for (uint8_t i = 0; i < len; i++ ) {
		uint8_t c = cmds[i];
		if (c == CMD_DELAY) {
			delay(2);
		} else {
			cmd(c);
		}
	}
}

void TinyLCD::data(uint8_t data) {
	digitalWrite(_pin_RS, HIGH);
	digitalWrite(_pin_CS, LOW);
	SPI.transfer(data);
	digitalWrite(_pin_CS, HIGH);
}

void TinyLCD::clear() {
	memset((void*)_vram, 0, sizeof(_vram));
}

void TinyLCD::flush(uint8_t offset) {
	for (int page = 0; page < PAGES; page++) {
		cmd(0xb0 + page);
		cmd(0x10);
		cmd(0x00);
		for (int col = 0; col < WIDTH; col++) {
			int col1 = (col + offset) % WIDTH;
			data(_vram[page][col1]);
		}
	}
}

void TinyLCD::drawPixel(int16_t x, int16_t y, bool set) {
#ifdef LCD_VERTICAL
	if (_vertical) {
		int16_t t = x;
		x = y;
		y = HEIGHT - t - 1;
	}
#endif
	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
	uint8_t* p = (uint8_t*)_vram + ((y / 8) * WIDTH + x);
	uint8_t mask = (1 << (y % 8));
	if (set) {
		*p |= mask;
	} else {
		*p &= ~mask;
	}
}

void TinyLCD::fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, bool set) {
	if (w == 1 && h == 1) {
		drawPixel(x, y, set);
		return;
	}
	if (x + w <= 0 || x >= _width || y + h <= 0 || y >= _height) return;
	if (x < 0) {
		w += x;
		x = 0;
	} else if (x + w > _width) {
		w -= x + w - _width;
	}
	if (y < 0) {
		h += y;
		y = 0;
	} else if (y + h > _height) {
		h -= y + h - _height;
	}
	for (uint8_t j = 0; j < h; j++) {
		for (uint8_t i = 0; i < w; i++) {
			drawPixel(x + i, y + j, set);
		}
	}
}

void TinyLCD::drawBitmap(const uint8_t* bitmap, int16_t x, int16_t y, uint8_t w, uint8_t h,
							uint8_t scaleX, uint8_t scaleY, bool bold, bool invert, bool overwrite) {
	const uint8_t* p = bitmap;
	uint8_t wbyte = w / 8;
	for (uint8_t j = 0; j < h; j++) {
		for (uint8_t i = 0; i < wbyte; i++) {
			uint8_t bits = *p++;
			if (bold) {
				bits |= (bits >> 1);
			}
			if (invert && !overwrite) {
				bits = ~bits;
			}
			for (uint8_t n = 0; n < 8; n++) {
				bool set = (bits & (1 << (7 - n))) != 0;
				bool reset = invert && overwrite && set;
				if (reset) {
					set = !set;
				}
				if (!overwrite || !invert && set || reset) {
					fillRect(x + scaleX * (i * 8 + n), y + scaleY * j, scaleX, scaleY, set);
				}
			}
		}
	}
}


void TinyLCD::print(int16_t x, int16_t y, char c) {
#ifdef LCD_TEXT_PROP
	x = calcCenter(x, _width, calcTextWidth(c));
	x -= calcPaddingLeft(c);
	bool textOverwrite = _textOverwrite || _textProp;
#else
	x = calcCenter(x, _width, _fontWidth * _textScaleX);
	#define textOverwrite _textOverwrite
#endif
	y = calcCenter(y, _height, _fontHeight * _textScaleY);

	drawBitmap(_fontBitmap + (c - ' ') * _fontByte, x, y, _fontWidth, _fontHeight, _textScaleX, _textScaleY, _textBold, _textInvert, textOverwrite);
}

void TinyLCD::print(int16_t x, int16_t y, char* str) {
#ifdef LCD_TEXT_PROP
	x = calcCenter(x, _width, calcTextWidth(str));
#else
	x = calcCenter(x, _width, _fontWidth * _textScaleX *  strlen(str));
#endif
	y = calcCenter(y, _height, _fontHeight * _textScaleY);
	while (true) {
		char c = *str++;
		if (c == '\0') break;
		print(x, y, c);
#ifdef LCD_TEXT_PROP
		if (_textProp) {
			x += (_fontWidthTable[c - 0x20] + _textBold + 1) * _textScaleX;
		} else {
			x += _fontWidth * _textScaleX;
		}
#else
		x += _fontWidth * _textScaleX;
#endif
	}
}

void TinyLCD::printf(int16_t x, int16_t y, const char* fmt, ...) {
   va_list args;
   va_start(args, fmt);
   vsnprintf(_textBuffer, TEXT_BUFFER_SIZE, fmt, args);
   va_end(args);
   print(x, y, _textBuffer);
}

#ifdef LCD_TEXT_PROP
uint8_t TinyLCD::calcTextWidth(char c) {
	if (!_textProp) return _fontWidth * _textScaleX;
	uint8_t w = _fontWidthTable[c - 0x20] + _textBold;
	return w * _textScaleX;
}

uint16_t TinyLCD::calcTextWidth(char* str) {
	if ( !_textProp) return _fontWidth * _textScaleX * strlen(str);
	uint16_t w = 0;
	while (char c = *str++) {
		if (w) {
			w++;
		}
		w += _fontWidthTable[c - 0x20] + _textBold;
	}
	return w * _textScaleX;
}

uint8_t TinyLCD::calcPaddingLeft(char c) {
	if ( !_textProp) return 0;
	uint8_t w = _fontWidthTable[c - 0x20];
	return (8 - w) / 2;
}

#endif

void TinyLCD::drawMessage(const char* msg) {
	clear();
	setTextStyle(true);
	setTextScale(1, 2);
	print(CENTER, CENTER, (char*)msg);
	flush();
}

