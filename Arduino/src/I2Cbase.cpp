#include "I2Cbase.h"
#include <Wire.h>

bool I2Cbase::_initialized = false;

void I2Cbase::begin() {
	if (!_initialized) {
		_initialized = true;
		Wire.begin();
		Wire.setClock(400000);
	}
}

bool I2Cbase::isConnected() {
	Wire.beginTransmission(_addr);
	return Wire.endTransmission() == 0;
}

void I2Cbase::i2cWrite(uint8_t value) {
	Wire.beginTransmission(_addr);
	Wire.write(value);
	Wire.endTransmission();
}

void I2Cbase::i2cWrite(char* s) {
	Wire.beginTransmission(_addr);
	Wire.write(s);
	Wire.endTransmission();
}

uint8_t I2Cbase::i2cRead() {
	Wire.requestFrom(_addr, (uint8_t)1);
	while (!Wire.available()) ;
	return Wire.read();
}

void I2Cbase::i2cWriteRegByte(uint8_t reg, uint8_t value) {
	Wire.beginTransmission(_addr);
	Wire.write(reg);
	Wire.write(value);
	Wire.endTransmission();
}

uint8_t I2Cbase::i2cReadRegByte(uint8_t reg) {
	Wire.beginTransmission(_addr);
	Wire.write(reg);
	Wire.endTransmission();
	Wire.requestFrom(_addr, 1);
	while (!Wire.available()) ;
	return Wire.read();
}

uint16_t I2Cbase::i2cReadRegWord(uint8_t reg) {
	Wire.beginTransmission(_addr);
	Wire.write(reg);
	Wire.endTransmission();
	Wire.requestFrom(_addr, 2);
	while (!Wire.available()) ;
	uint8_t lsb = (uint8_t)Wire.read();
	return Wire.read() << 8 | lsb;
}

void I2Cbase::i2cReadRegBytes(uint8_t reg, uint8_t* buffer, uint16_t len) {
	i2cWrite(reg);
	Wire.requestFrom(_addr, len);
	for (uint16_t i = 0; i < len; i++) {
		buffer[i] = Wire.read();
	}
}

void I2Cbase::i2cReadRegBytes(uint8_t reg1, uint8_t reg2, uint8_t* buffer, uint16_t len) {
	i2cWriteRegByte(reg1, reg2);
	Wire.requestFrom(_addr, len);
	for (uint16_t i = 0; i < len; i++) {
		buffer[i] = Wire.read();
	}
}
