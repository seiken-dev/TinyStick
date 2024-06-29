#include "TinyStick.h"
#include "playDigit.h"
#include "octave.h"

uint16_t digit_tones[] = { FREQ_C3, FREQ_C4, FREQ_D4, FREQ_E4, FREQ_F4, FREQ_G4, FREQ_A4, FREQ_B4, FREQ_C5, FREQ_D5, FREQ_G5 };

void playDigit(uint16_t num) {
	uint16_t n = num / 10;
	if (n > 0) {
		if (n >= 10) {
			n = 10; // FREQ_G5
		}
		buzzer.beep(digit_tones[n], 50);
	}
	buzzer.beep(digit_tones[num % 10], 100);
}
