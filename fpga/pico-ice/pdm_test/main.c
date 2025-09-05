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
volatile uint32_t* pdm_enable = (volatile uint32_t*)0x8000280;
volatile uint32_t* pdm_period = (volatile uint32_t*)0x8000284;
volatile uint32_t* pdm_select = (volatile uint32_t*)0x8000288;
volatile uint32_t* pdm_sample = (volatile uint32_t*)0x800028c;
volatile uint16_t* pdm_sample16 = (volatile uint16_t*)0x800028c;
volatile uint8_t* pdm_sample8 = (volatile uint8_t*)0x800028c;

#define BUFFER_LENGTH 1000
volatile int16_t buffer[BUFFER_LENGTH];
volatile uint32_t buffer_idx;

int main(void) {
    printf("\n-----------------------------------\n");

    // Set all outputs to regular mode (not debug)
    set_debug_sel(0xff);

    // gpio 4
    enable_all_outputs();

    // Set all output 6 to peripheral 10
    set_gpio_func(6, 10);

    for(int i=0; i<BUFFER_LENGTH; ++i)
    	    buffer[i] = 22222;

    *pdm_period = 14;
    *pdm_select = 0;
    *pdm_enable = 1;

    uint32_t t0 = read_time();
    uint32_t t1 = t0 + 100 * 1000;
    while (read_time() < t1)
    	    ;

    enable_interrupt(10);

    while (buffer_idx < 900)
	    ;
    *pdm_enable = 0;

    printf("BEGIN\n");
    for(int i=0; i<BUFFER_LENGTH; i+=8) {
	    for(int j=0; j<8; ++j) {
		    printf("%6d", buffer[i+j]);
	    }
	    printf("\n");
    }
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
