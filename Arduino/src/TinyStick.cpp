#include "TinyStick.h"

//
// Button class
//

static const uint8_t button_pins[BUTTONS_COUNT] = { PIN_BUTTON1, PIN_BUTTON2, PIN_BUTTON3, PIN_BUTTON4
#if (BUTTONS_COUNT == 5)
	, PIN_BUTTON5
#endif
};

Button btn1(PIN_BUTTON1);
Button btn2(PIN_BUTTON2);
Button btn3(PIN_BUTTON3);
Button btn4(PIN_BUTTON4);
#if (BUTTONS_COUNT == 5)
Button btn5(PIN_BUTTON5);
#endif

static Button* buttons[] = { &btn1, &btn2, &btn3, &btn4
#if (BUTTONS_COUNT == 5)
, &btn5
#endif
};

void Button::init() {
	for (uint8_t i = 0; i < BUTTONS_COUNT; i++) {
		buttons[i]->begin();
	}
}

void Button::updateAll() {
	for (uint8_t i = 0; i < BUTTONS_COUNT; i++) {
		buttons[i]->update();
	}
}

void Button::begin() {
	pinMode(_pin, INPUT_PULLUP);
	_state = read();
}

void Button::update(bool clear) {
	_statePrev = clear ? false : _state; // clear is for wake up
	_state = read();
	if (wasPressed()) {
		_tick = millis();
	} else if (wasReleased()) {
		_tick = 0;
	}
}

bool Button::read() {
	return digitalRead(_pin) == LOW;
}

bool Button::wasPressed() {
	return (!_statePrev && _state);
}

bool Button::wasReleased() {
	return (_statePrev && !_state);
}

bool Button::wasLongPressed(uint16_t elapse) {
	if (_tick && millis() - _tick >= elapse) {
		_tick = 0;
		return true;
	}
	return false;
}

void Button::waitForRelease() {
	while (read()) {}
	_statePrev = _state = false;
}

//
// Buzzer class
//
Buzzer buzzer;

void Buzzer::beep(uint16_t freq, uint16_t ms, bool isBlock) {
	tone(PIN_BUZZER, freq, ms);
	if (isBlock) {
		delay(ms);
	}
}

#ifdef TS_SLEEP

#include <avr/sleep.h>
//
// Sleep functions
//
// https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/cores/megatinycore/WInterrupts.c
// detachInterrupt() doesn't work, so rewrite it.

extern voidFuncPtr * intFunc[];

void tsAttachInterrupt(uint8_t pin, void (*userFunc)(void), uint8_t mode) {
	uint8_t bitpos = digitalPinToBitPosition(pin);
	if (bitpos == NOT_A_PIN) {
		return;
	}
    switch (mode) {
      case CHANGE:
        mode = PORT_ISC_BOTHEDGES_gc;
        break;
      case FALLING:
        mode = PORT_ISC_FALLING_gc;
        break;
      case RISING:
        mode = PORT_ISC_RISING_gc;
        break;
      case LOW:
        mode = PORT_ISC_LEVEL_gc;
        break;
      default:
        return;
    }

	cli();
    uint8_t port = digitalPinToPort(pin);
	if (intFunc[port] != nullptr) {
		if (userFunc != nullptr) {
			intFunc[port][bitpos] = userFunc;
		}
		PORT_t* portp = digitalPinToPortStruct(pin);
		volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portp, bitpos);
		*pin_ctrl_reg &= ~(PORT_ISC_gm);
		*pin_ctrl_reg |= mode;
	}
	sei();
}

void tsDetachInterrupt(uint8_t pin) {
	uint8_t bitpos = digitalPinToBitPosition(pin);
	if (bitpos == NOT_A_PIN) {
		return;
	}

	cli();
	uint8_t port = digitalPinToPort(pin);
	intFunc[port][bitpos] = nullptr;
	PORT_t* portp = digitalPinToPortStruct(pin);
	volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portp, bitpos);
	*pin_ctrl_reg &= ~(PORT_ISC_gm);
	sei();
}

static void wakeUp() {
}

void tsAttachSleepTriggers(bool isAttach) {
	for (int i = 0; i < BUTTONS_COUNT; i++) {
		if (isAttach) {
			tsAttachInterrupt(button_pins[i], wakeUp, LOW);
		} else {
			tsDetachInterrupt(button_pins[i]);
		}
	}
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

// FALLING mode only works with PX2(PIN_BUTTON1)&PX6, but LOW mode works with all buttons
void tsSetSleepTriggers(uint8_t mode) {
	for (int i = 0; i < BUTTONS_COUNT; i++) {
		tsSetInterruptMode(button_pins[i], mode);
	}
}

ISR(RTC_PIT_vect) {
	RTC.PITINTFLAGS = RTC_PI_bm;
}

void tsSetSleepTimer(SleepTime sleepTime) {
	RTC.CLKSEL     = RTC_CLKSEL_INT1K_gc;
	RTC.PITINTCTRL = RTC_PITEN_bm;
	RTC.PITCTRLA   = (sleepTime << 3) | RTC_PITEN_bm;

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void tsSleep() {
	sleep_enable();
	sleep_cpu();
	sleep_disable();
}

#endif // TS_SLEEP
