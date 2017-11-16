#include <stdio.h>
#include <stdint.h>

#include "efm32gg.h"
#include "graphics.h"

void __attribute__ ((interrupt)) TIMER1_IRQHandler() {  
    /* Clear interrupt flag */
    *TIMER1_IFC = 1;
}

void GPIO_Handler() {
    printf("Handler");
}

void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() { 
    GPIO_Handler();
}

void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() {
    GPIO_Handler();
}