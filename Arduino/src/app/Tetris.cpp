#include "Tetris.h"
#include "TinyLCD.h"
#include "octave.h"

static const uint8_t bmp_piece[8] = {
	0b11111110,
	0b11111110,
	0b11111110,
	0b11111110,
	0b11111110,
	0b11111110,
	0b11111110,
	0b00000000,
};

static const uint8_t bmp_piece_filled[8] = {
	0b11111111,
	0b11000001,
	0b10100001,
	0b10010001,
	0b10001001,
	0b10000101,
	0b10000011,
	0b11111111,
};

static constexpr uint8_t PIECE_SIZE = 8;
static constexpr uint8_t CELL_WIDTH = 6;
static constexpr uint8_t CELL_HEIGHT = 16;
static constexpr uint8_t CELL_SIZE = CELL_WIDTH * CELL_HEIGHT;
static constexpr uint8_t MINO_TYPE_COUNT = 7;
static constexpr int8_t CELL_MINO = 1;
static constexpr int8_t CELL_FILL = 2;
static constexpr int8_t CELL_FLASH = 3;
static constexpr int8_t CELL_FLASH2 = 4;

static constexpr uint16_t DELAY = 500;
static constexpr uint16_t DELAY_FALLING = 100;
static constexpr uint16_t DELAY_FLASH = 120;
static constexpr uint8_t FLASH_COUNT = 3;

static int8_t cells[CELL_SIZE];

static const uint8_t minoTypes[MINO_TYPE_COUNT] = {
	0b11110000,
	0b01100110,
	0b11000110,
	0b01101100,
	0b11101000,
	0b11100010,
	0b11100100,
};

static const uint8_t minoExtend[3][4] = {
	// Length L, R, Up, Down
	{ 0, 0, 1, 2 }, // type 0
	{ 0, 1, 0, 1 }, // type 1
	{ 0, 1, 1, 1 }, // type 2 .. 6
};

void Tetris::setActive(bool isActive) {
	TinyApp::setActive(isActive);
	if (isActive) {
		randomSeed(millis());
		random(1); // usually 0 at first
		start();
	}
}


void Tetris::start() {
	memset(cells, 0, sizeof(cells));
	throwIn();
	flashCount = 0;
}

void Tetris::throwIn() {
	minoType = random(MINO_TYPE_COUNT);
	minoX = CELL_WIDTH / 2;
	minoY = 1;
	minoRotate = 0;
	isFalling = false;
	isThrowIn = true;
	prevTick = millis();
}

void Tetris::update() {
	if (btn2.wasPressed()) {
		if (canMoveOrRotate(1, minoRotate)) {
			minoX++;
			buzzer.click();
		}
	} else if (btn3.wasPressed()) {
		if (canMoveOrRotate(-1, minoRotate)) {
			minoX--;
			buzzer.click();
		}
	} else if (btn1.wasLongPressed(500)) {
		isFalling = true;
		buzzer.beep(FREQ_E6, 100, false);
	} else if (btn1.wasReleased() && !isFalling) {
		if (minoType != 1) { // square
			uint8_t nextRotate = (minoRotate + 1) & 3;
			if (canMoveOrRotate(0, nextRotate)) {
				minoRotate = nextRotate;
				buzzer.click();
			}
		}
	}

	uint16_t elapse = (flashCount > 0) ? DELAY_FLASH : isFalling ? DELAY_FALLING : DELAY;
	unsigned long tick = millis();
	if (prevTick == 0 || tick - prevTick >= elapse) {
		prevTick = tick;
		if (flashCount) {
			flash();
			if (--flashCount == 0) {
				eraseLines();
				if (findLinedUp()) {
					flashCount = FLASH_COUNT * 2;
				} else {
					throwIn();
				}
			}
			drawCell();
			return;
		}

		minoY++;
		clearMino();
		int8_t r = putMino();
		if (r) {
			if (r < 0) {
				if (isThrowIn) {
					gameOver();
					return;
				}
				minoY--;
				clearMino();
				putMino();
			}
			landingMino();
			if (findLinedUp()) {
				flashCount = FLASH_COUNT * 2;
			} else {
				throwIn();
			}
		} else {
			isThrowIn = false;
		}
		drawCell();
	}
}

void Tetris::clearMino() {
	for (int16_t i = 0; i < CELL_SIZE; i++) {
		if (cells[i] == CELL_MINO) {
			cells[i] = 0;
		}
	}
}

void Tetris::landingMino() {
	for (int16_t i = 0; i < CELL_SIZE; i++) {
		if (cells[i] == CELL_MINO) {
			cells[i] = CELL_FILL;
		}
	}
}

int8_t Tetris::putMino() {
	int8_t result = 0;
	uint8_t mask = 0x80;
	uint8_t minoBits = minoTypes[minoType];

	int8_t x = 0;
	int8_t y = -1;
	bool isLanding = false;
	for (uint8_t i = 0; i < 8; i++) {
		if (minoBits & mask) {
			int8_t x1 = x;
			int8_t y1 = y;
			if (minoRotate == 1) {
				x1 = y;
				y1 = -x;
			} else if (minoRotate == 2) {
				x1 = -x;
				y1 = -y;
			} else if (minoRotate == 3) {
				x1 = y;
				y1 = x;
			}
			x1 += minoX;
			y1 += minoY;
			int8_t index = y1 * CELL_WIDTH + x1;
			if (index >= 0 && index < CELL_SIZE) {
				if (cells[index]) {
					result = -1;
					break;
				}
				cells[index] = CELL_MINO;
				if (y1 >= CELL_HEIGHT - 1) {
					result = 1;
				}
			}
		}
		mask >>= 1;
		y++;
		if (i == 3) {
			x = 1;
			y = -1;
		}
	}
	return result;
}

bool Tetris::canMoveOrRotate(int8_t dx, int8_t rotate) {
	const uint8_t* extend = minoExtend[minoType < 3 ? minoType : 2];
	int8_t lx, rx;
	switch (rotate) {
		case 0:
			lx = extend[0];
			rx = extend[1];
			break;
		case 1:
			lx = extend[2];
			rx = extend[3];
			break;
		case 2:
			lx = extend[1];
			rx = extend[0];
			break;
		case 3:
			lx = extend[3];
			rx = extend[2];
			break;
	}
	bool r = (minoX + dx - lx >= 0) && (minoX + dx + rx < CELL_WIDTH);
//	Serial.printf("X=%d dx=%d lx=%d rx=%d (%d)\n", minoX, dx, lx, rx, r);
	return r;
}

bool Tetris::findLinedUp() {
	bool r = false;
	int8_t i = 0;
	for (int8_t y = 0; y < CELL_HEIGHT; y++) {
		int8_t n = 0;
		for (int8_t x = 0; x < CELL_WIDTH; x++) {
			if (cells[i]) {
				n++;
			}
			i++;
		}
		if (n == CELL_WIDTH) {
			r = true;
			for (int8_t x = 0; x < CELL_WIDTH; x++) {
				cells[y * CELL_WIDTH + x] = CELL_FLASH;
			}
		}
	}
	return r;
}

void Tetris::flash() {
	for (int8_t i = 0; i < CELL_SIZE; i++) {
		uint8_t cell = cells[i];
		if (cell == CELL_FLASH) {
			cells[i] = CELL_FLASH2;
		} else if (cell == CELL_FLASH2) {
			cells[i] = CELL_FLASH;
		}
	}
	buzzer.beep((flashCount & 1) ? FREQ_G6 : FREQ_C7, 100, false);
}

void Tetris::eraseLines() {
	for (int8_t y = 0; y < CELL_HEIGHT; y++) {
		int8_t x0 = y * CELL_WIDTH;
		if (cells[x0] >= CELL_FLASH) {
			for (int8_t i = x0 - 1; i >= 0; i--) {
				cells[i + CELL_WIDTH] = cells[i];
			}
		}
	}
}

void Tetris::drawCell() {
	LCD.clear();
	int8_t i = 0;
	for (int8_t y = 0; y < CELL_HEIGHT; y++) {
		for (int8_t x = 0; x < CELL_WIDTH; x++) {
			uint8_t cell = cells[i];
			if (cell) {
				const uint8_t* bmp = (cell >= CELL_FILL) ? bmp_piece_filled : bmp_piece;
				LCD.drawBitmap(bmp, y * PIECE_SIZE, LCD.HEIGHT - (x + 1) * PIECE_SIZE, 8, 8, 1, 1, cell == CELL_FLASH2);
			}
			i++;
		}
	}
	LCD.fillRect(0, 0, 1, LCD.HEIGHT);
	LCD.fillRect(0, 0, LCD.WIDTH, 1);
	LCD.fillRect(LCD.WIDTH - 1, 0, 1, LCD.HEIGHT);
	LCD.fillRect(0, LCD.HEIGHT - 1, LCD.WIDTH, 1);
	LCD.flush();
}

void Tetris::gameOver() {
	for (int i = 0; i < 5; i++) {
		buzzer.beep((i & 1) ? FREQ_B2 : FREQ_C3, 150);
	}
	start();
}

