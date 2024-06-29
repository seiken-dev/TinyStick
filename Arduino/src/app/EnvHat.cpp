// https://github.com/m5stack/M5-ProductExampleCodes/blob/master/Hat/ENVII_HAT/Arduino/ENVII_HAT/SHT3X.cpp

#include "EnvHat.h"
#include "I2Cbase.h"
#include "TinyLCD.h"

static I2Cbase sht30(0x44);

bool EnvHat::begin() {
	I2Cbase::begin();
	if (!sht30.isConnected()) {
		_isReady = false;
		return false;
	}
	return true;
}

#ifdef SLEEP_MODE
void EnvHat::setActive(bool isActive) {
	TinyApp::setActive(isActive);
	if (_isReady) {
		tsAttachSleepTriggers(isActive);
		tsSetSleepTimer(isActive ? SLEEP_1S : SLEEP_OFF);
	}
}
#endif

bool EnvHat::toNextApp() {
	return btn1.wasPressed();
}

void EnvHat::update() {
	unsigned long tick = millis();
	if (read()) {
		drawEnv();

#ifdef DEBUG_SERIAL
		uint16_t elapse = millis() - tick;
		Serial.printf("temp=%d, hum=%d (%d)\n", temp, humidity, elapse);
#endif
	}
#ifdef SLEEP_MODE
	tsSetSleepTriggers(LOW);
	tsSleep();
	tsSetSleepTriggers(FALLING);
#endif
}


bool EnvHat::read() {
	uint8_t data[6];

	sht30.i2cReadRegBytes(0x2C, 0x06, data, 6);

#ifdef NO_FLOAT
	temp = ((int32_t)data[0] << 8 | data[1]) * 175 / 6554 - 450;
	humidity =  ((int32_t)data[3] << 8 | data[4]) * 100 / 6554;
#else
	temp = ((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45;
	humidity = ((((data[3] * 256.0) + data[4]) * 100) / 65535.0);
#endif

	return true;
}

static const uint8_t bmp_circle[3] = {
	0b00010000,
	0b00101000,
	0b00010000,
};

void EnvHat::drawEnv() {
	LCD.clear();

	LCD.setTextScale(3, 4);
	LCD.printf(0, 10, "%d", temp / 10);
	LCD.printf(64, 10, "%d", humidity / 10);
	LCD.setTextScale(1, 2);
	LCD.printf(44, 24, ".%d", temp % 10);
	LCD.printf(108, 24, ".%d", humidity % 10);
	LCD.setTextScale(2, 2);
	LCD.drawBitmap(bmp_circle, 40, 2, 8, 3, 2, 2);
	LCD.print(50, 6, 'C');
	LCD.print(112, 4, '%');

	LCD.flush();
}
