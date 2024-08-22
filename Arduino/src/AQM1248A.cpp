#include "AQM1248A.h"
#include <SPI.h>

//
// AQM1248A class
//
// https://github.com/SWITCHSCIENCE/samplecodes/tree/master/AQM1248A_breakout/Arduino/AQM1248A_lib

#define CMD_DELAY 0xFF

const static uint8_t initCmds[] = {0xAE, 0xA0, 0xC8, 0xA3, 0x2C, CMD_DELAY, 0x2E, CMD_DELAY, 0x2F, 0x23, 0x81, 0x1C, 0xA4, 0x40, 0xA6, 0xAF };

void AQM1248A::begin(uint8_t pin_RS, uint8_t pin_CS, uint8_t pin_BL) {
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

void AQM1248A::cmd(uint8_t cmd) {
	digitalWrite(_pin_RS, LOW);
	digitalWrite(_pin_CS, LOW);
	SPI.transfer(cmd);
	digitalWrite(_pin_CS, HIGH);
}

void AQM1248A::cmds(const uint8_t cmds[], uint8_t len) {
	for (uint8_t i = 0; i < len; i++ ) {
		uint8_t c = cmds[i];
		if (c == CMD_DELAY) {
			delay(2);
		} else {
			cmd(c);
		}
	}
}

void AQM1248A::data(uint8_t data) {
	digitalWrite(_pin_RS, HIGH);
	digitalWrite(_pin_CS, LOW);
	SPI.transfer(data);
	digitalWrite(_pin_CS, HIGH);
}

void AQM1248A::clear() {
	memset((void*)_vram, 0, sizeof(_vram));
}

void AQM1248A::flush(uint8_t offset) {
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

void AQM1248A::drawPixel(int16_t x, int16_t y, bool set) {
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

