#pragma once

#include "TinyApp.h"

class AccelDemo : public TinyApp {
public:
	virtual const char* getAppName() { return "Bounce Ball"; }
	virtual bool begin();
	virtual void update();
	void reset();

	virtual void setActive(bool isActive);

	void printAccel(uint16_t elapse);

private:
	int16_t marbleX, marbleY;
	int16_t marbleVX, marbleVY;

	void reflect();
};