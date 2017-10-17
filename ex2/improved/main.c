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
	setupNVIC();
	makeSongs();
	start_song = 0;

	//Deepsleep when nothing done
	*SCR = 0x6;
	//Waiting for interrupt
	__asm("wfi");

}

void setupNVIC()
{

	*GPIO_EXTIPSELL = 0x22222222; 	// external interrupt select
	*GPIO_EXTIFALL = 0xFF;			// interrupt on 1->0 transition
	*GPIO_EXTIRISE = 0xFF; 			// interrupt on 0->1 transition
	*GPIO_IEN = 0xFF; 				// enable interrupt generation 
	*ISER0 = 0x802; 				// enable interrupt handling for even and odd buttons
	*GPIO_IFC = *GPIO_IF; 			// clear interrupt flags
}