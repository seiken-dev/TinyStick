#include "TinyStick.h"
#include "TinyLCD.h"
#include "TinyApp.h"
#include "font8x8.h"

// Simply list the TinyApp derived classes and switch them in order.

#include "app/EnvHat.h"
EnvHat envHat;

#include "app/KitchenTimer.h"
KitchenTimer kitchenTimer;

#include "app/AccelDemo.h"
AccelDemo accelDemo;

#include "app/FontTest.h"
FontTest fontTest;

#include "app/Tetris.h"
Tetris tetris;

#include "app/Maze3D.h"
Maze3D maze3D;

void drawTitle() {
	LCD.clear();
	LCD.setTextProp(true);
	LCD.setTextScale(2, 3);
	LCD.print(0, 0, "Welcome to");
	LCD.fillRect(5, 25, 35, 22);
	LCD.setTextStyle(true, true);
	LCD.setTextScale(1, 2);
	LCD.print(9, 28, "Tiny");
	LCD.setTextStyle(true);
	LCD.setTextScale(2, 3);
	LCD.print(42, 25, "Stick!");
	LCD.flush();
	LCD.reset();
}

void flash(uint16_t ms = 20) {
	digitalWrite(PIN_LED, HIGH);
	delay(ms);
	digitalWrite(PIN_LED, LOW);
}

//
// setup
//
void setup() {
	pinMode(PIN_LED, OUTPUT);
	delay(100);

#ifdef DEBUG_SERIAL
    Serial.begin(115200);
#endif

	LCD.begin(PIN_LCD_RS, PIN_LCD_CS, PIN_LCD_BL);
#ifdef LCD_TEXT_PROP
	LCD.setFont(font8x8, font8x8_width);
#else
	LCD.setFont(font8x8);
#endif

	Button::init();
	buzzer.begin();

	drawTitle();
	delay(200);
	digitalWrite(PIN_LED, HIGH);
	buzzer.beep(2000, 150);
	buzzer.beep(1000, 150);
	digitalWrite(PIN_LED, LOW);
	delay(500);

	TinyApp::startApp(0);

#ifdef DEBUG_SERIAL
	Serial.println("start");
#endif
}

//
// loop
//
#define MIN_DELAY 20

void loop() {
	unsigned long tick = millis();
	uint32_t elapse = 0;

	Button::updateAll();

	if (btnSide.wasPressed()) {
		buzzer.click();
	}
	if (btnSide.wasLongPressed()) {
		LCD.backLight(!LCD.isBackLight());
		Serial.printf("BL=%d\n", LCD.isBackLight());
		btnSide.waitForRelease();
	} else if (btnSide.wasReleased()) {
		TinyApp::nextApp();
	} else {
		TinyApp* app = TinyApp::getApp();
		if (app && app->toNextApp()) {
			buzzer.click();
			TinyApp::nextApp();
		}
	}

	TinyApp::updateApp();

	flash();
	if (elapse < MIN_DELAY) {
		delay(MIN_DELAY - elapse);
	}
}
