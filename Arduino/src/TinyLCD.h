#pragma once

#include <stdint.h>
#include "TinyStickDef.h"

#define TEXT_BUFFER_SIZE 20

//
// TinyLCD class (AQM1248A driver)
//
class TinyLCD {
public:
	static const uint8_t WIDTH = 128;
	static const uint8_t HEIGHT = 48;
	static const uint8_t PAGES = 6;
	static const int16_t CENTER = INT16_MAX;

	TinyLCD() {}

	void begin(uint8_t pin_RS, uint8_t pin_CS, uint8_t pin_BL);
	void cmd(uint8_t cmd);
	void cmds(const uint8_t cmds[], uint8_t len);
	void data(uint8_t data);
	void backLight(bool on) {
		digitalWrite(_pin_BL, on);
		_backLight = on;
	}
	bool isBackLight() { return _backLight; }

	void clear();
	void flush(uint8_t offset = 0);
	void drawPixel(int16_t x, int16_t y, bool set = 1);
	void fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, bool set = 1);
	void drawBitmap(const uint8_t* bitmap, int16_t x, int16_t y, uint8_t w = 8, uint8_t h = 8,
			uint8_t scaleX = 1, uint8_t scaleY = 1, bool bold = false, bool invert = false, bool overwrite = false);
#ifdef LCD_VERTICAL
	void setVertical(bool vertical = false) {
		_vertical = vertical;
		if (vertical) {
			_width = HEIGHT;
			_height = WIDTH;
		} else {
			_width = WIDTH;
			_height = HEIGHT;
		}
	}
#endif
	void print(int16_t x, int16_t y, char c);
	void print(int16_t x, int16_t y, char* str);
	void printf(int16_t x, int16_t y, const char* fmt, ...);
	void drawMessage(const char* msg);

#ifdef LCD_TEXT_PROP
	void setFont(const uint8_t* fontBitmap, const uint8_t* fontWidthTable = nullptr, uint8_t fontWidth = 8, uint8_t fontHeight = 8) {
		_fontBitmap = fontBitmap;
		_fontWidthTable = fontWidthTable;
#else
	void setFont(const uint8_t* fontBitmap, uint8_t fontWidth = 8, uint8_t fontHeight = 8) {
		_fontBitmap = fontBitmap;
#endif
		_fontWidth = fontWidth;
		_fontHeight = fontHeight;
		_fontByte = fontWidth / 8 * fontHeight;
	}
	void setTextScale(uint8_t scaleX = 1, uint8_t scaleY = 1) {
		_textScaleX = scaleX;
		_textScaleY = scaleY;
	}
	void setTextStyle(bool bold = false, bool invert = false, bool overwrite = false) {
		_textBold = bold;
		_textInvert = invert;
		_textOverwrite = overwrite;
	}
	void setTextProp(bool textProp = false) {
#ifdef LCD_TEXT_PROP
		if (_fontWidthTable) {
			_textProp = textProp;
		}
#endif
	}

	void reset() {
		setTextScale();
		setTextStyle();
		setTextProp();
#ifdef LCD_VERTICAL
		setVertical();
#endif
	}


protected:
	uint8_t _pin_RS;
	uint8_t _pin_CS;
	uint8_t _pin_BL;

	bool _backLight = false;

	uint8_t _vram[PAGES][WIDTH];
	char _textBuffer[TEXT_BUFFER_SIZE];
#ifdef LCD_VERTICAL
	bool _vertical = false;
	uint8_t _width = WIDTH;
	uint8_t _height = HEIGHT;
#endif

	const uint8_t* _fontBitmap = nullptr;
#ifdef LCD_TEXT_PROP
	const uint8_t* _fontWidthTable = nullptr;
#endif
	uint8_t _fontWidth = 8;
	uint8_t _fontHeight = 8;
	uint8_t _fontByte = 8;
	uint8_t _textScaleX = 1;
	uint8_t _textScaleY = 1;
	bool _textBold = false;
	bool _textInvert = false;
	bool _textOverwrite = false;
#ifdef LCD_TEXT_PROP
	bool _textProp = false;
#endif

	int16_t calcCenter(int16_t v, uint8_t vMax, uint8_t size) {
		return (v != CENTER) ? v : (vMax - size) / 2;
	}
#ifdef LCD_TEXT_PROP
	uint8_t calcTextWidth(char c);
	uint16_t calcTextWidth(char* str);
	uint8_t calcPaddingLeft(char c);
#endif
};

extern TinyLCD LCD;