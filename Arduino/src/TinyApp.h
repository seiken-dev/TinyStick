#pragma once

#include "TinyStick.h"

//
// TinyApp class (Base class for switching multiple apps)
//
constexpr uint8_t MAX_TINYAPPS = 8;

class TinyApp {
public:
	TinyApp();
	virtual const char* getAppName() { return nullptr; }
	virtual bool begin() { return true; }
	virtual void update() {}
	virtual bool toNextApp() { return false; }

	virtual void setActive(bool isActive);
	bool isActive() { return _isActive; }
	bool isReady() { return _isReady; }
	virtual void drawTitle();

	static int8_t tinyAppIndex;
	static uint8_t tinyAppCount;

	static void startApp(uint8_t index);
	static void nextApp();
	static TinyApp* getApp() { return tinyAppCount ? tinyApps[tinyAppIndex] : nullptr; }
	static void updateApp();

protected:
	static TinyApp* tinyApps[MAX_TINYAPPS];

	bool _isActive = false;
	bool _isReady = true;
	bool _initialized = false;
};
