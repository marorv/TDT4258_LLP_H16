#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "song.h"

void setupDAC()
{

	 *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_DAC0; // enable DAC clock
	 *DAC0_CTRL = 0x50010; // prescale DAC clock
	 //activateDAC(); Don't need these from the start
}

//Why have DAC on when we're not using it?
void deactivateDAC()
{
	*DAC0_CH0CTRL = 0x0; // deactivate left audio channel
	*DAC0_CH1CTRL = 0x0; // deactivate right audio channel 
}

//Presumably want to use it
void activateDAC()
{
	*DAC0_CH0CTRL = 0x01; // enable left audio channel
	*DAC0_CH1CTRL = 0x01; //enable right audio channel 

}

