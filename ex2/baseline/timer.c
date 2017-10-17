#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/*
 * function to setup the timer 
 */
void setupTimer(uint16_t period)
{

	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_TIMER1; //enable clock

	*TIMER1_TOP = period; //set period of timer

}

void startTimer()
{
		*TIMER1_CMD = 0x01; //start timer
}

void stopTimer()
{
		*TIMER1_CMD = 0x02; //stop timer
}
