#include <stdint.h>
#include <stdbool.h>

#include "song.h"
#include "efm32gg.h"
#include "interrupt_handlers.h"

void GPIO_Handler();

//According to internet, we can't have code outside of functions in C. Moving to main
//start_song = 0;

/*
 * TIMER1 interrupt handler 
 */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	/*
	 * TODO feed new samples to the DAC remember to clear the pending
	 * interrupt by writing 1 to TIMER1_IFC 
	 */
	
	*TIMER1_IFC = 1; // clear interrupt

		
}

void startTimer()
{
		*TIMER1_CMD = 0x01; //start timer
}

void stopTimer()
{
		*TIMER1_CMD = 0x00; //stop timer
}


