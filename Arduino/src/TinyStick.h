#pragma once

#include <Arduino.h>
#include "TinyStickDef.h"

//
// Button class
//
class Button {
public:
	Button(uint8_t pin) : _pin(pin), _statePrev(false) {}

	static void init();
	static void updateAll();

	void update(bool clear = false);
	void begin();
	bool read();
	bool wasPressed();
	bool wasReleased();
	bool wasLongPressed(uint16_t elapse = 1000);
	void waitForRelease();
	void clear() {
		_state = _statePrev = false;
	}

public:
	uint8_t _pin;
	bool _state;
	bool _statePrev;
	bool _changed = false;
	unsigned long _tick;
};

extern Button btn1;
extern Button btn2;
extern Button btn3;
extern Button btn4;
extern Button btn5;

//
// Buzzer class
//
class Buzzer {
public:
	void begin() {
		pinMode(PIN_BUZZER, OUTPUT);
	}
	void beep(uint16_t freq, uint16_t ms, bool isBlock = true);
	void click(bool isBlock = false) {
		beep(3520, 20, isBlock);
	}
};

extern Buzzer buzzer;

#ifdef TS_SLEEP

//
// Sleep functions
//
enum SleepTime {
	SLEEP_OFF, SLEEP_3_9MS, SLEEP_7_8MS, SLEEP_15_6MS, SLEEP_31_25MS, SLEEP_62_5MS, SLEEP_125MS,
	SLEEP_250MS, SLEEP_500MS, SLEEP_1S, SLEEP_2S, SLEEP_4S, SLEEP_8S, SLEEP_16S, SLEEP_32S,
};

void tsAttachInterrupt(uint8_t pin, void (*userFunc)(void), uint8_t mode);
void tsDetachInterrupt(uint8_t pin);
inline void tsSetInterruptMode(uint8_t pin, uint8_t mode) {
	tsAttachInterrupt(pin, nullptr, mode);
}

void tsAttachSleepTriggers(bool isAttach);
void tsSetSleepTriggers(uint8_t mode);

void tsSetSleepTimer(SleepTime sleepTime);
void tsSleep();

#endif // TS_SLEEP

