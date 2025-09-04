#include <stddef.h>
#include <gpio.h>
#include <timer.h>
#include <stdio.h>
#include <csr.h>
#include <uart.h>
#include <math.h>

#define printf uart_printf

// PDM audio input
// base address is 0x800_0280 (0x800_0000 + 0x40*10)
volatile uint32_t* pdm_ctrl = (volatile uint32_t*)0x8000280;
volatile uint32_t* pdm_clkp = (volatile uint32_t*)0x8000284;
volatile uint32_t* pdm_pcmw = (volatile uint32_t*)0x8000288;
volatile uint16_t* pdm_pcmw16 = (volatile uint16_t*)0x8000288;
volatile uint8_t* pdm_pcmw8 = (volatile uint8_t*)0x8000288;

#define BUFFER_LENGTH 64
volatile int16_t buffer[BUFFER_LENGTH];
volatile uint32_t buffer_idx;

int main(void) {
    printf("main\n");

    // Set all outputs to regular mode (not debug)
    set_debug_sel(0xff);

    // gpio 4
    enable_all_outputs();

    // Set all output 6 to peripheral 10
    set_gpio_func(6, 10);

    enable_interrupt(10);

    *pdm_clkp = 14;
    *pdm_ctrl = 1;

    printf("sampling...");
    //while (!buffer[BUFFER_LENGTH-1])
    while (buffer_idx < 60)
    	    ;
    printf("stopping...");
    *pdm_ctrl = 0;

    printf("BEGIN\n");
    for(int i=0; i<BUFFER_LENGTH; ++i)
	    printf("%6d\n", buffer[i]);
    printf("END\n");

    for(;;)
    	    ;
    
    uint32_t tick_ms_prev = 0;
    while (1) {
        const uint32_t tick_ms = read_time()*64/(14*1000);

        if ((tick_ms - tick_ms_prev) > 1000) {
            	for(int i=0; i<BUFFER_LENGTH; ++i)
			printf("%6d\n", buffer[i]);
		printf("\n");
            tick_ms_prev = tick_ms;
        }
    }

    return 0;
}
