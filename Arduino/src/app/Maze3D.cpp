#include "Maze3D.h"
#include "TinyLCD.h"
#include "octave.h"

#define btnUp btn2
#define btnLeft btn1
#if (BUTTONS_COUNT == 5)
#define btnDown btn3
#define btnRight btn4
#else
#define btnRight btn3
#endif

static constexpr uint8_t POS_X = 0;
static constexpr uint8_t WIDTH = 96;

void Maze3D::setActive(bool isActive) {
	TinyApp::setActive(isActive);
	if (isActive) {
		randomSeed(millis());
		start();
		draw();
	}
}

void Maze3D::update() {
	int8_t r = -1;
	if (btnUp.wasPressed()) {
		r = movePlayer(MAZE_DIR_UP);
	} else if (btnLeft.wasPressed()) {
		turnPlayer(false);
		r = 1;
	} else if (btnRight.wasPressed()) {
		turnPlayer(true);
		r = 1;
	}
#if (BUTTONS_COUNT == 5)
	else if (btnDown.wasPressed()) {
		r = movePlayer(MAZE_DIR_DOWN);
	}
#endif
	if (r > 0) {
		buzzer.click();
		draw();
		if (_px == _goalX && _py == _goalY) {
			goalIn();
			start();
			draw();
		}
	} else if (r == 0) {
		buzzer.beep(FREQ_C2, 100);
		buzzer.beep(FREQ_D2, 100);
	}
}

void Maze3D::start() {
	_px = MAZE_SIZE / 2;
	_py = MAZE_SIZE - 2;
	_pdir = MAZE_DIR_UP;
	_goalX = random(MAZE_SIZE / 2 - 1) * 2 + 1;
	_goalY = 1;

	createMaze();
}

void Maze3D::draw() {
	LCD.clear();
	drawMaze();
	draw3DMaze();
	drawGoal("GOAL");
	LCD.flush();
}

void Maze3D::goalIn() {
	buzzer.beep(FREQ_B4, 240);
	delay(120);
	buzzer.beep(FREQ_A4, 120);
	buzzer.beep(FREQ_B4, 480);
}

void Maze3D::createMaze() {
	uint16_t mazeN[MAZE_SIZE];

	memset(_maze, 0, sizeof(_maze));
	memset(mazeN, 0, sizeof(mazeN));
	fillWall(0, 0, MAZE_SIZE, 1);
	fillWall(0, MAZE_SIZE - 1, MAZE_SIZE, 1);
	fillWall(0, 0, 1, MAZE_SIZE);
	fillWall(MAZE_SIZE - 1, 0, 1, MAZE_SIZE);

	for (uint8_t y = 2; y < MAZE_SIZE; y += 2) {
		for (uint8_t x = 2; x < MAZE_SIZE; x += 2) {
			if (isWall(_maze, x, y)) continue;
			memset(mazeN, 0, sizeof(mazeN));
			uint8_t x1 = x;
			uint8_t y1 = y;
			bool isDone = false;
			for (;;) {
				setWall(mazeN, x1, y1);
				uint8_t dir = random(4);
				uint8_t x2, y2;
				int8_t dx, dy;
				uint8_t n = 0;
				for (; n < 4; n++) {
					setDxDy(dir, dx, dy);
					x2 = x1 + dx * 2;
					y2 = y1 + dy * 2;
					if (isWall(_maze, x2, y2)) {
						isDone = true;
						break;
					}
					if (!isWall(mazeN, x2, y2)) break; // next
					dir = (++dir) % 4;
				}
				if (n == 4) {
					memset(mazeN, 0, sizeof(mazeN));
					x1 = x;
					y1 = y;
					continue;
				}
				x1 = x2;
				y1 = y2;
				setWall(mazeN, x1 - dx, y1 - dy);
				if (isDone) {
					setWall(mazeN, x1, y1);
					for (uint8_t i = 0; i < MAZE_SIZE; i++) {
						_maze[i] |= mazeN[i];
					}
					break;
				}
			}
		}
	}
}

void Maze3D::setWall(uint16_t* maze, uint8_t x, uint8_t y, bool isSet) {
	uint16_t bit = (1 << (MAZE_SIZE - 1 - x));
	if (isSet) {
		maze[y] |= bit;
	} else {
		maze[y] &= ~bit;
	}
}

bool Maze3D::isWall(uint16_t* maze, uint8_t x, uint8_t y) {
	uint16_t bit = (1 << (MAZE_SIZE - 1 - x));
	return (maze[y] & bit) != 0;
}

void Maze3D::fillWall(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, bool isSet) {
	for (uint8_t y = y0; y < y0 + h; y++) {
		for (uint8_t x = x0; x < x0 + w; x++) {
			setWall(_maze, x, y, isSet);
		}
	}
}

void Maze3D::drawMaze() {
	LCD.drawBitmap((uint8_t*)_maze, LCD.WIDTH - (MAZE_SIZE + 1) * 2, 0, 16, MAZE_SIZE, 2, 2);
}

void Maze3D::draw3DMaze() {
	constexpr uint8_t PAD_X = 6;
	constexpr uint8_t PAD_Y = 3;
	constexpr uint8_t VERTEX_COUNT = 6;

	uint8_t vx[VERTEX_COUNT];
	uint8_t vy[VERTEX_COUNT];

	vx[0] = 0;
	vy[0] = 0;
	for (uint8_t i = 1; i < VERTEX_COUNT; i++) {
		vx[i] = WIDTH / 2 - ((WIDTH / 2 - PAD_X) / i);
		vy[i] = LCD.HEIGHT / 2 - ((LCD.HEIGHT / 2 - PAD_Y) / i);
	}
	for (uint8_t s = 0; s < 2; s++) { // left or right side
		for (uint8_t i = 1; i < VERTEX_COUNT; i++) {
			if (isWallNearPlayer(i - 1, s)) {
				if (s) {
					LCD.drawLine(WIDTH - vx[i - 1] + POS_X, vy[i - 1], WIDTH - vx[i], vy[i]);
					LCD.drawLine(WIDTH - vx[i - 1] + POS_X, LCD.HEIGHT - vy[i - 1], WIDTH - vx[i], LCD.HEIGHT - vy[i]);
				} else {
					LCD.drawLine(vx[i - 1] + POS_X, vy[i - 1], vx[i] + POS_X, vy[i]);
					LCD.drawLine(vx[i - 1] + POS_X, LCD.HEIGHT - vy[i - 1], vx[i] + POS_X, LCD.HEIGHT - vy[i]);
				}
			} else {
				if (s) {
					LCD.drawHLine(WIDTH - vx[i] + POS_X, vy[i], vx[i] - vx[i - 1]);
					LCD.drawHLine(WIDTH - vx[i] + POS_X, LCD.HEIGHT - vy[i], vx[i] - vx[i - 1]);
				} else {
					LCD.drawHLine(vx[i - 1] + POS_X, vy[i], vx[i] - vx[i - 1]);
					LCD.drawHLine(vx[i - 1] + POS_X, LCD.HEIGHT - vy[i], vx[i] - vx[i - 1]);
				}
			}
			LCD.drawVLine(vx[i] + POS_X, vy[i], LCD.HEIGHT - vy[i] * 2);
			LCD.drawVLine(WIDTH - vx[i] + POS_X, vy[i], LCD.HEIGHT - vy[i] * 2);
			int8_t fx, fy;
			setDxDy(_pdir, fx, fy);
			if (isWall(_maze, _px + fx * i, _py + fy * i)) {
				LCD.drawHLine(vx[i] + POS_X, vy[i], WIDTH - vx[i] * 2);
				LCD.drawHLine(vx[i] + POS_X, LCD.HEIGHT - vy[i], WIDTH - vx[i] * 2);
				break;
			}
		}
	}
}

bool Maze3D::isWallNearPlayer(uint8_t dist, bool isRight) {
	int8_t fx, fy, sx, sy;
	setDxDy(_pdir, fx, fy);
	setDxDy((_pdir + ((isRight) ? 1 : 3)) & 3, sx, sy);
	return isWall(_maze, _px + sx + fx * dist, _py + sy + fy * dist);
}

void Maze3D::setDxDy(uint8_t dir, int8_t& dx, int8_t& dy) {
	dx = 0;
	dy = 0;
	switch (dir) {
	case MAZE_DIR_UP:
		dy = -1;
		break;
	case MAZE_DIR_RIGHT:
		dx = 1;
		break;
	case MAZE_DIR_DOWN:
		dy = 1;
		break;
	case MAZE_DIR_LEFT:
		dx = -1;
		break;
	}
}

bool Maze3D::movePlayer(uint8_t dir) {
	int8_t dx, dy;
	dir = (_pdir + dir) & 3;
	setDxDy(dir, dx, dy);
	if (isWall(_maze, _px + dx, _py + dy)) return false;
	_px += dx;
	_py += dy;
	return true;
}

void Maze3D::turnPlayer(bool isRight) {
	if (isRight) {
		_pdir++;
	} else {
		_pdir--;
	}
	_pdir &= 3;
}

void Maze3D::drawGoal(const char* str) {
	int8_t dx, dy;
	setDxDy(_pdir, dx, dy);
	int8_t dist = 0;
	if (_px == _goalX && dx == 0) {
		if (_goalY < _py) {
			if (dy > 0) return;
			dist = _py - _goalY;
		} else if (_goalY > _py) {
			if (dy < 0) return;
			dist = _goalY - _py;
		}
	} else if (_py == _goalY && dy == 0) {
		if (_goalX < _px) {
			if (dx > 0) return;
			dist = _px - _goalX;
		} else if (_goalX > _px) {
			if (dx < 0) return;
			dist = _goalX - _px;
		}
	} else {
		return;
	}
	if (dist <= 1) {
		if (isWall(_maze, _goalX + dx, _goalY + dy)) {
			uint8_t scale = 2 - dist;
			LCD.setTextScale(scale, scale);
			LCD.setTextProp(true);
			uint8_t textWidth = LCD.calcTextWidth((char* )str);
			uint8_t x = (WIDTH - textWidth) / 2 + POS_X;
			uint8_t y = (LCD.HEIGHT - 7 * scale) / 2;
			LCD.print(x, y, str);
		}
	}
}
