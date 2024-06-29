#pragma once

#include "TinyApp.h"

#define NO_FLOAT

#ifdef TS_SLEEP
#define SLEEP_MODE
#endif

class EnvHat : public TinyApp {
public:
	virtual const char* getAppName() { return "Env Hat"; }
	virtual bool begin();
	virtual void update();
	virtual bool toNextApp();

#ifdef SLEEP_MODE
	virtual void setActive(bool isActive);
#endif

	bool read();

	void drawEnv();

#ifdef NO_FLOAT
	int16_t temp; // 10x value
	int16_t humidity;
#else
	float temp;
	float humidity;
#endif
};
