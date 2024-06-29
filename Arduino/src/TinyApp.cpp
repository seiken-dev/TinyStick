#include "TinyApp.h"
#include "TinyLCD.h"

//
// TinyApp class
//
int8_t TinyApp::tinyAppIndex = -1;
uint8_t TinyApp::tinyAppCount = 0;

TinyApp* TinyApp::tinyApps[MAX_TINYAPPS];

TinyApp::TinyApp() {
	if (tinyAppCount >= MAX_TINYAPPS) {
		this->_isReady = false;
	} else {
		tinyApps[tinyAppCount++] = this;
	}
}

void TinyApp::startApp(uint8_t index) {
	if (index < tinyAppCount) {
		tinyAppIndex = index;
		TinyApp* app = tinyApps[index];
		app->setActive(true);
		if (!app->isReady()) {
			nextApp();
		}
	}
}

void TinyApp::nextApp() {
	if (tinyAppIndex >= 0 && tinyAppIndex < tinyAppCount) {
		for(;;) {
			tinyApps[tinyAppIndex]->setActive(false);
			if (++tinyAppIndex >= tinyAppCount) {
				tinyAppIndex = 0;
			}
			tinyApps[tinyAppIndex]->setActive(true);
			if (tinyApps[tinyAppIndex]->isReady()) break;
		}
	}
}
void TinyApp::updateApp() {
	if (tinyAppIndex >= 0 && tinyAppIndex < tinyAppCount) {
		tinyApps[tinyAppIndex]->update();
	}
}

void TinyApp::setActive(bool isActive) {
	_isActive = isActive;
	if (_isActive) {
		if (!_initialized) {
			begin();
			_initialized = true;
		}
		if (_isReady) {
			drawTitle();
		}
	}
}

void TinyApp::drawTitle() {
	const char* appName = getAppName();
	if (appName) {
		LCD.clear();
		LCD.setTextProp(true);
		LCD.setTextScale(1, 2);
		LCD.setTextStyle(true);
		LCD.print(LCD.CENTER, LCD.CENTER, (char*)appName);
		LCD.flush();
		unsigned long tick = millis();
		while (millis() - tick < 750) {
			if (btn4.read()) break;
			delay(10);
		}
		LCD.clear();
		LCD.reset();
		LCD.flush();
	}
}
