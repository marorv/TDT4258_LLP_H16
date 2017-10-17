#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

void setupDAC()
{

	 *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_DAC0; // enable DAC clock
	 *DAC0_CTRL = 0x50010; // prescale DAC clock
	 *DAC0_CH0CTRL = 0x01; // enable left audio channel
	 *DAC0_CH1CTRL = 0x01; //enable right audio channel 

}
