#include <Arduino.h>
#include <SPI.h>
#include "TinyLCD.h"

//
// TinyLCD class
//

TinyLCD LCD;

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
//			uint8_t bits = *p++;
			uint8_t bits = *(p + (wbyte - i - 1));
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
		p += wbyte;
	}
}

void TinyLCD::print(int16_t x, int16_t y, char c) {
#ifdef LCD_TEXT_PROP
	x = calcCenter(x, _width, calcTextWidth(c));
	x -= calcPaddingLeft(c) * _textScaleX;
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
	uint8_t len = 0;
	while (char c = *str++) {
		w += _fontWidthTable[c - 0x20];
		len++;
	}
	w += (len - 1) * (_textBold + 1);
	return w * _textScaleX;
}

uint8_t TinyLCD::calcPaddingLeft(char c) {
	if ( !_textProp) return 0;
	uint8_t w = _fontWidthTable[c - 0x20];
	return (8 - w) / 2;
}

#endif

#ifdef LCD_DRAW_LINE

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

void TinyLCD::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool set) {
	int16_t dx = abs(x1 - x0);
	int16_t dy = abs(y1 - y0);
	int8_t sx = (x0 < x1) ? 1 : -1;
	int8_t sy = (y0 < y1) ? 1 : -1;
	int16_t err = dx - dy;

	for (;;) {
		LCD.drawPixel(x0, y0, set);
		// if (x0 == x1 && y0 == y1) break;
		if ((sx > 0 ? x0 >= x1 : x0 <= x1) && (sy > 0 ? y0 >= y1 : y0 <= y1)) break;
		if (err + err > -dy) {
			err -= dy;
			x0 += sx;
		}
		if (err + err < dx) {
			err += dx;
			y0 += sy;
		}
	}
}
#endif

void TinyLCD::drawMessage(const char* msg) {
	clear();
	setTextStyle(true);
	setTextScale(1, 2);
	setTextProp(true);
	print(CENTER, CENTER, (char*)msg);
	flush();
	reset();
}

