#pragma once

#include "TinyApp.h"

#ifdef TS_SLEEP
#define SLEEP_MODE
#endif
//#undef SLEEP_MODE

class KitchenTimer : public TinyApp {
public:
	virtual const char* getAppName() { return "Kitchen Timer"; }
	virtual void update();
	virtual void setActive(bool isActive);

private:
	uint8_t _minute = 3;

#ifdef SLEEP_MODE
	int16_t _halfSecCounter = -1;
#else
	uint16_t _halfSecPrev;
	unsigned long _starttick = 0;
#endif

	void drawMinute();
	void drawSecond(uint16_t sec, bool colon);
	void playAlert();
};
