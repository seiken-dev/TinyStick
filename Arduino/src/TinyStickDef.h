#pragma once

#define DEBUG_SERIAL

#define TS_SLEEP // sleep support
#define LCD_VERTICAL // vertical mode
#define LCD_TEXT_PROP // proportional

//#define PLAY_DIGIT // play digit by tone
#define USE_EEPROM // save settings in EEPROM

#define EEPROM_KITCHEN_TIMER_MINUTE 1

#define PIN_LED PIN_PA7
#define PIN_BUZZER PIN_PB5

#define PIN_LCD_RS PIN_PC0
#define PIN_LCD_CS PIN_PC1
#define PIN_LCD_BL PIN_PB4

#define PIN_BUTTON1 PIN_PC2
#define PIN_BUTTON2 PIN_PC3
#ifdef ARDUINO_attinyxy7
#define PIN_BUTTON3 PIN_PC4
#define PIN_BUTTON4 PIN_PC5
#else
#define PIN_BUTTON3 PIN_PB4
#define PIN_BUTTON4 PIN_PB5
#endif

