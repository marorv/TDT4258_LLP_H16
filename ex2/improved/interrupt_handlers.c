#include <stdint.h>
#include <stdbool.h>

#include "song.h"
#include "efm32gg.h"
#include "interrupt_handlers.h"

void GPIO_Handler();

void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	
	*TIMER1_IFC = 1; // clear interrupt

	if(start_song) playSong();
		
}

void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{

	GPIO_Handler();
}

void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{

	GPIO_Handler();
}

void GPIO_Handler()
{
	uint16_t buttons_pressed = *GPIO_PC_DIN << 8;

	*GPIO_IFC = *GPIO_IF;

	if(buttons_pressed != 0xFF00){
		
		startTimer();
		*SCR = 0x2;
		activateDAC();
	
	} else {
		//stopTimer();
		//*SCR = 0x6;
		//deactivateDAC();
	}

	selectSong(buttons_pressed);
	*GPIO_PA_DOUT = buttons_pressed;	
	
}