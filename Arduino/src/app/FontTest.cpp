#include "FontTEst.h"
#include "TinyLCD.h"

void FontTest::setActive(bool isActive) {
	TinyApp::setActive(isActive);

	if (isActive) {
		drawFonts();
	}
}

void FontTest::update() {
	if (btn1.wasPressed()) {
		if (++_scale > 4) {
			_scale = 1;
		}
		_startIndex = 0;
		buzzer.click();
		drawFonts();
	} else if (btn2.wasPressed()) {
		if (scroll(-1)) {
			buzzer.click();
			drawFonts();
		}
	} else if (btn3.wasPressed()) {
		if (scroll(1)) {
			buzzer.click();
			drawFonts();
		}
	}
}

void FontTest::drawFonts() {
	LCD.clear();
	LCD.setTextScale(_scale, _scale);

	uint8_t size = _scale * 8;
	uint8_t c = 0x20 + _startIndex;
	for (uint8_t y = 0; y <= LCD.HEIGHT - size; y += size) {
		for (uint8_t x = 0; x <= LCD.WIDTH - size; x += size) {
			LCD.print(x, y, c++);
			if (c > 0x7F) break;
		}
	}
	LCD.flush();
}

bool FontTest::scroll(int8_t dy) {
	if (_scale == 1) return false;
	uint8_t cx = LCD.WIDTH / (_scale * 8);
	uint8_t cy = LCD.HEIGHT / (_scale * 8);
	int8_t index = _startIndex + cx * dy;
	if (index < 0 || index + cx * (cy - 1) >= 96) return false;
	_startIndex = index;
	return true;
}