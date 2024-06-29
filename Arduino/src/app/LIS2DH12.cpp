#include "LIS2DH12.h"

#define LIS2DH12_WHO_AM_I		0x0F
#define LIS2DH12_CTRL_REG1 		0x20
#define LIS2DH12_CTRL_REG4 		0x23
#define LIS2DH12_STATUS_REG 	0x27
#define LIS2DH12_OUT_X_L		0x28

#define LIS2DH12_ID				0x33

union CtrlReg1_t {
	struct {
		uint8_t Xen 	: 1;
		uint8_t Yen 	: 1;
		uint8_t Zen 	: 1;
		uint8_t LPen	: 1;
		uint8_t ODR 	: 4;
	};
	uint8_t uint8;
};

union CtrlReg4_t {
	struct {
		uint8_t SIM 	: 1;
		uint8_t ST		: 2;
		uint8_t HR		: 1;
		uint8_t FS		: 2;
		uint8_t BLE		: 1;
		uint8_t BDU		: 1;
	};
	uint8_t uint8;
};

union RawData_t {
  int16_t int16[3];
  uint8_t uint8[6];
};

bool LIS2DH12::begin() {
	if (!isConnected()) return false;
	// Enable Block Data Update

	CtrlReg4_t ctrlReg4;
	ctrlReg4.uint8 = i2cReadRegByte(LIS2DH12_CTRL_REG4);
	ctrlReg4.BDU = 1;
	i2cWriteRegByte(LIS2DH12_CTRL_REG4, ctrlReg4.uint8);

	setDataRate(LIS2DH12_DATA_RATE);
	setScale(LIS2DH12_SCALE);
	setMode(LIS2DH12_MODE);
	return true;
}

bool LIS2DH12::isConnected() {
	if (I2Cbase::isConnected()) {
		if (i2cReadRegByte(LIS2DH12_WHO_AM_I) == LIS2DH12_ID) {
			return true;
		}
	}
	return false;
}

bool LIS2DH12::available() {
	const uint8_t STATUS_REG_XYXDA = 0x08;

	return (i2cReadRegByte(LIS2DH12_STATUS_REG) & STATUS_REG_XYXDA) != 0;
}

void LIS2DH12::waitForNewData() {
	while (!available()) {
		delay(1);
	}
}

bool LIS2DH12::read(bool isBlock) {
	if (!available()) {
		if (!isBlock) return false;
		waitForNewData();
	}
	readAccelData();
	return true;
}

void LIS2DH12::readAccelData() {
	RawData_t rawData;
//	memset(rawData.uint8, 0, sizeof(rawData.uint8) * 6);

	i2cReadRegBytes(LIS2DH12_OUT_X_L | 0x80, rawData.uint8, 6);

	rawX = rawData.int16[0];
	rawY = rawData.int16[1];
	rawZ = rawData.int16[2];
}

void LIS2DH12::setDataRate(uint8_t dataRate) {
	CtrlReg1_t ctrlReg1;
	ctrlReg1.uint8 = i2cReadRegByte(LIS2DH12_CTRL_REG1);
	ctrlReg1.ODR = dataRate;
	i2cWriteRegByte(LIS2DH12_CTRL_REG1, ctrlReg1.uint8);
}

void LIS2DH12::setScale(uint8_t scale) {
	CtrlReg4_t ctrlReg4;
	ctrlReg4.uint8 = i2cReadRegByte(LIS2DH12_CTRL_REG4);
	ctrlReg4.FS = scale;
	i2cWriteRegByte(LIS2DH12_CTRL_REG4, ctrlReg4.uint8);
}

void LIS2DH12::setMode(uint8_t mode) {
	CtrlReg1_t ctrlReg1;
	CtrlReg4_t ctrlReg4;

	ctrlReg1.uint8 = i2cReadRegByte(LIS2DH12_CTRL_REG1);
	ctrlReg4.uint8 = i2cReadRegByte(LIS2DH12_CTRL_REG4);
	ctrlReg1.LPen = 0;
	ctrlReg4.HR = 0;
	if (mode == LIS2DH12_HR_12bit) {
		ctrlReg4.HR = 1;
	} else if (mode == LIS2DH12_LP_8bit) {
		ctrlReg1.LPen = 1;
	}

	i2cWriteRegByte(LIS2DH12_CTRL_REG1, ctrlReg1.uint8);
	i2cWriteRegByte(LIS2DH12_CTRL_REG4, ctrlReg4.uint8);
}

#if LIS2DH12_SCALE == LIS2DH12_2g
#define SCALE_MUL (1)
#elif LIS2DH12_SCALE == LIS2DH12_4g
#define SCALE_MUL (2)
#elif LIS2DH12_SCALE == LIS2DH12_8g
#define SCALE_MUL (4)
#elif LIS2DH12_SCALE == LIS2DH12_16g
#define SCALE_MUL (12)
#endif

#ifdef NO_FLOAT
int16_t LIS2DH12::convertAccel(int16_t raw) {
//	return raw * SCALE_MUL / 16;
	return (raw * SCALE_MUL) >> 4;
}

#else
float LIS2DH12::convertAccel(int16_t raw) {
	return (float)raw * SCALE_MUL / 16;
}

#endif // NO_FLOAT
