#pragma once

#include <Arduino.h>

//
// I2Cbase class (Base class for I2C devices)
//
class I2Cbase {
public:
	static void begin();

	I2Cbase(uint8_t addr) : _addr(addr) {}
	bool isConnected();

	void i2cWrite(uint8_t value);
	void i2cWrite(char* s);
	uint8_t i2cRead();
	void i2cWriteRegByte(uint8_t reg, uint8_t value);
	uint8_t i2cReadRegByte(uint8_t reg);
	uint16_t i2cReadRegWord(uint8_t reg);
	void i2cReadRegBytes(uint8_t reg, uint8_t *buffer, uint16_t len);
	void i2cReadRegBytes(uint8_t reg1, uint8_t reg2, uint8_t* buffer, uint16_t len);

protected:
	uint8_t _addr;
	static bool _initialized;
};