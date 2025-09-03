#include <stddef.h>
#include <gpio.h>
#include <timer.h>
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

    printf("main-start\n");

    // Set all outputs to regular mode (not debug)
    set_debug_sel(0xff);

    // Set all output 6 to peripheral 10
    set_gpio_func(6, 10);

    // Enable PDM clock
    *pdm_clkp = 14;
    *pdm_ctrl = 1;

    // Enable interrupt
    enable_interrupt(10);
    
    printf("loop-start\n");

    static uint32_t previous_status = 0;
    // Main loop
    while (1) {
#if 1
        // Manually compensate for running on 14 Mhz instead of 64 Mhz
        const uint32_t tick_ms = read_time()*64/(14*1000);
#else
        // XXX: no output in this case
        const uint32_t tick_ms = get_mtime()/1000;
#endif
        if ((tick_ms - previous_status) > 3000) {
            // NOTE: 3rd argument to printf not interpolated, just gives 0
            printf("loop-status time_ms=%d rate=%d sample_counter=%d \n",
                tick_ms, sample_counter/(tick_ms/1000), sample_counter);
            previous_status = tick_ms;
        }
    }

    return 0;
}
