#include "KitchenTimer.h"
#include "TinyLCD.h"

#ifdef USE_EEPROM
#include <EEPROM.h>
#endif

#ifdef PLAY_DIGIT
#include "playDigit.h"
#endif

void KitchenTimer::setActive(bool isActive) {
	TinyApp::setActive(isActive);

	if (isActive) {
#ifdef USE_EEPROM
		int8_t n = EEPROM.read(EEPROM_KITCHEN_TIMER_MINUTE);
		if (n != -1) {
			_minute = n;
		}
#endif
		drawMinute();
	}

#ifdef SLEEP_MODE
	tsAttachSleepTriggers(isActive);
	tsSetSleepTimer(SLEEP_OFF);
#endif
}

void KitchenTimer::update() {
#ifdef SLEEP_MODE
	tsSetSleepTriggers(LOW);
	tsSleep();
	tsSetSleepTriggers(FALLING);

	btn1.update(true);
	btn2.update(true);
	btn3.update(true);
#endif

	uint8_t minutePrev = _minute;

	if (btn1.wasPressed()) {
#ifdef SLEEP_MODE
		buzzer.click(true);
		if (_halfSecCounter < 0) {
			_halfSecCounter = 0;
			tsSetSleepTimer(SLEEP_500MS);
#ifdef USE_EEPROM
			EEPROM.write(EEPROM_KITCHEN_TIMER_MINUTE, _minute);
#endif
		} else {
			_halfSecCounter = -1;
			drawMinute();
			tsSetSleepTimer(SLEEP_OFF);
		}
		btn1.waitForRelease();
#else
		if (_starttick == 0) {
			_starttick = millis();
			_halfSecPrev = 0;
#ifdef USE_EEPROM
			EEPROM.write(EEPROM_KITCHEN_TIMER_MINUTE, _minute);
#endif
		} else {
			_starttick = 0;
			drawMinute();
		}
		buzzer.click();
#endif
	} else if (btn2.wasPressed()) {
		if (_minute < 99) {
			_minute++;
		}
	} else if (btn3.wasPressed()) {
		if (_minute > 1) {
			_minute--;
		}
	}
	if (_minute != minutePrev) {
#ifdef SLEEP_MODE
		buzzer.click(true);
		if (_halfSecCounter < 0) {
			drawMinute();
		}
		btn2.waitForRelease();
		btn3.waitForRelease();

#else
		if (_starttick == 0) {
			drawMinute();
		}
  #ifndef PLAY_DIGIT
		buzzer.click();
  #endif
#endif
	}

#ifdef SLEEP_MODE
	if (_halfSecCounter >= 0) {
		_halfSecCounter++;
		int16_t halfSec =  _minute * 120 - _halfSecCounter;
		drawSecond(halfSec / 2, halfSec & 1);
		if (halfSec <= 0) {
			playAlert();
			drawMinute();
			tsSetSleepTimer(SLEEP_OFF);
		}
	}

#else // SLEEP_MODE
	if (_starttick != 0) {
		uint16_t halfSec =  (uint16_t)((millis() - _starttick) / 500);
		if (halfSec != _halfSecPrev) {
			_halfSecPrev = halfSec;
			if (halfSec > 1) {
				uint16_t sec = _minute * 60 - halfSec / 2;
				drawSecond(sec, !(halfSec & 1));
				if (sec == 0) {
					_starttick = 0;
					playAlert();
					drawMinute();
				}
			}
		}
	}
#endif // SLEEP_MODE
}

void KitchenTimer::drawMinute() {
	LCD.clear();
	LCD.setTextScale(4, 5);
	LCD.printf(LCD.CENTER, LCD.CENTER, "%d", _minute);
	LCD.flush();
#ifdef PLAY_DIGIT
	playDigit(_minute);
#endif
}

void KitchenTimer::drawSecond(uint16_t sec, bool colon) {
	LCD.clear();
	if (sec < 60) {
		LCD.setTextScale(4, 5);
		LCD.printf(LCD.CENTER, LCD.CENTER, "%d", sec);
	} else {
		if (colon) {
			LCD.setTextScale(2, 2);
			LCD.print(LCD.CENTER, LCD.CENTER, ':');
		}
		LCD.setTextScale(4, 5);
		LCD.printf(-2, LCD.CENTER, "%2d", sec / 60);
		LCD.printf(68, LCD.CENTER, "%02d", sec % 60);
	}
	LCD.flush();
#ifdef PLAY_DIGIT
	if (colon) {
		if (sec >= 60) {
			if ((sec % 10) == 0) {
				uint16_t m = sec / 60;
				playDigit(m * 10 + (sec - m * 60) / 10);
			} else {
				playDigit(sec / 60);
			}
		} else {
			playDigit(sec % 60);
		}
	}
#endif
}

void KitchenTimer::playAlert() {
	for (uint8_t i = 0; i < 5; i++) {
		buzzer.beep(880, 500);
		delay(500);
	}
}