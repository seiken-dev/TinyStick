#pragma once

#include "TinyApp.h"

class FontTest : public TinyApp {
public:
	virtual const char* getAppName() { return "Font Test"; }
	virtual void update();
	virtual void setActive(bool isActive);

private:
	uint8_t _startIndex = 0;
	uint8_t _scale = 1;
	void drawFonts();
	bool scroll(int8_t dy);
};