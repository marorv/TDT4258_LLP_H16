#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"


//function to set up GPIO mode and interrupts
void setupGPIO()
{

	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;	// enable GPIO clock
	*GPIO_PA_CTRL = 0x02;	// set high drive strength
	*GPIO_PA_MODEH = 0x55555555; // set pins A8-15 as output 
	*GPIO_PA_DOUT |= 0xFF00; // turn off LEDs D0-D8

	*GPIO_PC_MODEL = 0x33333333; // set pins C0-7 to input
	*GPIO_PC_DOUT = 0xFF; // enable internal pull-ups
}
