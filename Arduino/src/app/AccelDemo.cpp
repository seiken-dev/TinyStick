#include "AccelDemo.h"
#include "TinyLCD.h"
#include "LIS2DH12.h"

static LIS2DH12 accel(0x18);

constexpr uint8_t MARBLE_SIZE = 8;

static const uint8_t bmp_marble[8] = {
	0b00111100,
	0b01111110,
	0b11111111,
	0b11011011,
	0b11111111,
	0b11100111,
	0b01111110,
	0b00111100,
};

bool AccelDemo::begin() {
	I2Cbase::begin();
	return accel.begin();
}

void AccelDemo::reset() {
	marbleX = LCD.WIDTH / 2;
	marbleY = LCD.HEIGHT / 2;
	marbleVX = marbleVY = 0;
}

void AccelDemo::setActive(bool isActive) {
	TinyApp::setActive(isActive);
	if (isActive) {
		reset();
	}
}

void AccelDemo::update() {
	unsigned long tick = millis();
	if (accel.read()) {
		marbleVX += accel.getX() / 64;
		marbleVY -= accel.getY() / 64;
		marbleX += marbleVX;
		marbleY += marbleVY;
		if (marbleX < 0 || marbleX >= LCD.WIDTH) {
			if (marbleX < 0) {
				marbleX = 0;
			} else {
				marbleX = LCD.WIDTH - 1;
			}
			marbleVX = -marbleVX / 2;
			reflect();
		}
		if (marbleY < 0 || marbleY >= LCD.HEIGHT) {
			if (marbleY < 0) {
				marbleY = 0;
			} else {
				marbleY = LCD.HEIGHT - 1;
			}
			marbleVY = -marbleVY / 2;
			reflect();
		}
		LCD.clear();
		LCD.drawBitmap(bmp_marble, marbleX - MARBLE_SIZE / 2, marbleY - MARBLE_SIZE / 2);
		printAccel((uint16_t)(millis() - tick));
		LCD.flush();
	}
	if (btn1.wasPressed()) {
		reset();
	}
}

void AccelDemo::reflect() {
	buzzer.beep(1760, 20, false);
}

void AccelDemo::printAccel(uint16_t elapse) {
	int16_t accelX = accel.getX();
	int16_t accelY = accel.getY();
	int16_t accelZ = accel.getZ();

	LCD.setTextScale(1, 1);
//	LCD.printf(0, 0,  "X=%d", accelX);
//	LCD.printf(0, 16, "Y=%d", accelY);
//	LCD.printf(0, 32, "Z=%d", accelZ);
	LCD.printf(0, 40, "X%-5dY%-5dZ%-5d", accelX, accelY, accelZ);
#ifdef DEBUG_SERIAL
	Serial.printf("X=%d, Y=%d, X=%d (%d)\n", accelX, accelY, accelZ, elapse);
#endif
}
