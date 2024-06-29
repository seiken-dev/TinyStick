// https://github.com/sparkfun/SparkFun_LIS2DH12_Arduino_Library

#pragma once

#include "I2Cbase.h"

#define NO_FLOAT

#define LIS2DH12_POWER_DOWN		0
#define LIS2DH12_ODR_1Hz		1
#define LIS2DH12_ODR_10Hz		2
#define LIS2DH12_ODR_25Hz		3
#define LIS2DH12_ODR_50Hz		4
#define LIS2DH12_ODR_100Hz		5
#define LIS2DH12_ODR_200Hz		6
#define LIS2DH12_ODR_400Hz		7
#define LIS2DH12_ODR_1620Hz_LP	8
#define LIS2DH12_ODR_1344Hz_5376Hz_LP	9

#define LIS2DH12_DATA_RATE LIS2DH12_ODR_10Hz

#define LIS2DH12_2g		0
#define LIS2DH12_4g		1
#define LIS2DH12_8g		2
#define LIS2DH12_16g	3

#define LIS2DH12_SCALE LIS2DH12_2g

#define LIS2DH12_HR_12bit	0
#define LIS2DH12_NM_10bit	1
#define LIS2DH12_LP_8bit	2

#define LIS2DH12_MODE LIS2DH12_HR_12bit


//
// LIS2DH12 class (3-Axis I2C Accelerometer)
//
class LIS2DH12 : public I2Cbase {
public:
	LIS2DH12(uint8_t addr = 0x19) : I2Cbase(addr) {}

	bool begin();
	bool isConnected();
	bool available();
	void waitForNewData();
	void readAccelData();
	bool read(bool isBlock = true);

#ifdef NO_FLOAT
	int16_t getX() { return convertAccel(rawX); }
	int16_t getY() { return convertAccel(rawY); }
	int16_t getZ() { return convertAccel(rawZ); }
#else
	float getX() { return convertAccel(rawX); }
	float getY() { return convertAccel(rawX); }
	float getZ() { return convertAccel(rawX); }
#endif
	int16_t getRawX() { return rawX; }
	int16_t getRawY() { return rawY; }
	int16_t getRawZ() { return rawZ; }

	void setDataRate(uint8_t dataRate);
	void setScale(uint8_t scale);
	void setMode(uint8_t mode);

protected:
	int16_t rawX;
	int16_t rawY;
	int16_t rawZ;

#ifdef NO_FLOAT
	int16_t convertAccel(int16_t raw);
#else
	float convertAccel(int16_t raw);
#endif
};
