#include <stdio.h>
#include <stdint.h>

#include "efm32gg.h"
#include "graphics.h"

void GPIO_Handler() {
    printf("Handler");
}

void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() { 
    GPIO_Handler();
}

void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() {
    GPIO_Handler();
}