#include <stddef.h>
#include <gpio.h>
#include <stdio.h>
#include <csr.h>
#include <uart.h>

#define printf uart_printf


// PDM audio input
// base address is 0x800_0280 (0x800_0000 + 0x40*10)
volatile uint32_t* pdm_ctrl = (volatile uint32_t*)0x8000280;
volatile uint32_t* pdm_clkp = (volatile uint32_t*)0x8000284;
volatile uint32_t* pdm_pcmw = (volatile uint32_t*)0x8000288;

static int64_t sample_counter = 0;

void tqv_user_interrupt10(void) {

    // Read sample. This also clears the interrupt
    int32_t sample = *pdm_pcmw;
    (void)sample; // TODO: put into a buffer

    sample_counter += 1;
}

int main(void) {

    // Set all outputs to regular mode (not debug)
    set_debug_sel(0xff);

    // Set all outputs to peripheral 10
    for (int i = 0; i < 8; ++i) {
        set_gpio_func(i, 10);
    }

    // Enable PDM clock
    *pdm_clkp = 14;

    // Enable interrupt
    enable_interrupt(10);
    
    static uint32_t previous_status = 0;
    // Main loop
    while (1) {
        const uint32_t tick_us = read_time();

        if ((tick_us - previous_status) > 1000000) {
            printf("loop-status time_ms=%d sample_counter=%d", tick_us/1000, sample_counter);
            previous_status = tick_us;
        }
    }

    return 0;
}
