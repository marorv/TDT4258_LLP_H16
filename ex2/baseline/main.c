#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "song.h"

//The period between sound samples, in clock cycles 
#define SAMPLE_PERIOD 317

//Declaration of peripheral setup functions 
void setupTimer(uint16_t period);
void setupDAC();
void setupGPIO();
void setupNVIC();


int main(void)
{
	setupGPIO();
	setupDAC();
	setupTimer(SAMPLE_PERIOD);
	start_song = 0;
	makeSongs();

	uint16_t buttons_pressed;
	
	while (1)
	{	
		buttons_pressed = *GPIO_PC_DIN << 8;
		*GPIO_PA_DOUT = buttons_pressed;
		selectSong(buttons_pressed);
		if(start_song) playSong();
	}

}