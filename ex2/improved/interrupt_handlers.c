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

	if(start_song) playSong();
		
}

/*
 * GPIO even pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
	/*
	 * TODO handle button pressed event, remember to clear pending
	 * interrupt 
	 */

	GPIO_Handler();
}

/*
 * GPIO odd pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
	/*
	 * TODO handle button pressed event, remember to clear pending
	 * interrupt 
	 */

	GPIO_Handler();
}

void GPIO_Handler()
{
	*GPIO_IFC = *GPIO_IF;

	uint16_t buttons_pressed = *GPIO_PC_DIN << 8;

	*GPIO_PA_DOUT = buttons_pressed;

	selectSong(buttons_pressed);

}

void startTimer()
{
		*TIMER1_CMD = 0x01; //start timer
}

void stopTimer()
{
		*TIMER1_CMD = 0x00; //stop timer
}


