#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "song.h"

/*
 * TODO calculate the appropriate sample period for the sound wave(s) you 
 * want to generate. The core clock (which the timer clock is derived
 * from) runs at 14 MHz by default. Also remember that the timer counter
 * registers are 16 bits. 
 */
/*
 * The period between sound samples, in clock cycles 
 */
#define SAMPLE_PERIOD 317

/*
 * Declaration of peripheral setup functions 
 */
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
	start_song = 0;
	makeSongs();

	//Deepsleep mode
	//HPF deactivated in deepsleep, so only normal sleep when playing sound: 0x2
	//DEEPSLEEP: =0x6 = 0b110
	// sleep: = 0x2 = 0b010

	//Assembly code call
	//__asm("wfi");

	//Deepsleep when nothing done
	*SCR = 0x2;


}

void setupNVIC()
{
	/*
	 * TODO use the NVIC ISERx registers to enable handling of
	 * interrupt(s) remember two things are necessary for interrupt
	 * handling: - the peripheral must generate an interrupt signal - the
	 * NVIC must be configured to make the CPU handle the signal You will
	 * need TIMER1, GPIO odd and GPIO even interrupt handling for this
	 * assignment. 
	 */

	*GPIO_EXTIPSELL = 0x22222222; // external interrupt select
	*GPIO_EXTIFALL = 0xFF;	// interrupt on 1->0 transition
	*GPIO_EXTIRISE = 0xFF; // interrupt on 0->1 transition
	*GPIO_IEN = 0xFF; // enable interrupt generation 
	*ISER0 = 0x802; // enable interrupt handling for even and odd buttons
	*GPIO_IFC = *GPIO_IF; // clear interrupt flags
}


/*
 * if other interrupt handlers are needed, use the following names:
 * NMI_Handler HardFault_Handler MemManage_Handler BusFault_Handler
 * UsageFault_Handler Reserved7_Handler Reserved8_Handler
 * Reserved9_Handler Reserved10_Handler SVC_Handler DebugMon_Handler
 * Reserved13_Handler PendSV_Handler SysTick_Handler DMA_IRQHandler
 * GPIO_EVEN_IRQHandler TIMER0_IRQHandler USART0_RX_IRQHandler
 * USART0_TX_IRQHandler USB_IRQHandler ACMP0_IRQHandler ADC0_IRQHandler
 * DAC0_IRQHandler I2C0_IRQHandler I2C1_IRQHandler GPIO_ODD_IRQHandler
 * TIMER1_IRQHandler TIMER2_IRQHandler TIMER3_IRQHandler
 * USART1_RX_IRQHandler USART1_TX_IRQHandler LESENSE_IRQHandler
 * USART2_RX_IRQHandler USART2_TX_IRQHandler UART0_RX_IRQHandler
 * UART0_TX_IRQHandler UART1_RX_IRQHandler UART1_TX_IRQHandler
 * LEUART0_IRQHandler LEUART1_IRQHandler LETIMER0_IRQHandler
 * PCNT0_IRQHandler PCNT1_IRQHandler PCNT2_IRQHandler RTC_IRQHandler
 * BURTC_IRQHandler CMU_IRQHandler VCMP_IRQHandler LCD_IRQHandler
 * MSC_IRQHandler AES_IRQHandler EBI_IRQHandler EMU_IRQHandler 
 */
