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

// Storage of audio data
#define BUFFER_LENGTH 128
static int16_t buffer[BUFFER_LENGTH];
uint32_t buffer_idx = 0;

// Debug/profiling
static uint32_t sample_counter = 0;
static uint32_t interrupt_cycles = 0;

void tqv_user_interrupt10(void) {

    const uint32_t interrupt_start = read_cycle();

    // Read sample. This also clears the interrupt
    int16_t sample = *pdm_pcmw;
    // uint8_t sample = *pdm_pcmw8;
    // uint16_t sample = *pdm_pcmw16;
    (void)sample; // TODO: put into a buffer

#if 1
    if (buffer_idx > BUFFER_LENGTH-1) {
        buffer_idx = 0;
    }    
    buffer[buffer_idx++] = sample;
#endif

    sample_counter += 1;
    // timing
    interrupt_cycles = (read_cycle() - interrupt_start);
}

uint64_t compute_soundlevel() {

    // non-optimize Root-Mean-Square (RMS)
    uint64_t acc = 0;
    // XXX: assumes that buffer is already filled
    for (int i=0; i<BUFFER_LENGTH; i++) {
        const int16_t sample = buffer[i];
        //acc += (sample * sample);
        acc += sample;
    }
    // sqrtf no symbol
    //const float rms = sqrtf(acc / BUFFER_LENGTH);
    const uint64_t mean = (acc / BUFFER_LENGTH);
    return mean;
}

int main(void) {

    printf("main-start\n");

    // Set all outputs to regular mode (not debug)
    set_debug_sel(0xff);

    // Set all output 6 to peripheral 10
    set_gpio_func(6, 10);

    // Enable PDM clock
    *pdm_clkp = 24; // FIXME: this should be 14
    *pdm_ctrl = 1;

    // Enable interrupt
    enable_interrupt(10);
    
    printf("loop-start\n");

    static uint32_t previous_status = 0;
    // Main loop
    while (1) {
        // Manually compensate for running on 14 Mhz instead of 64 Mhz
        const uint32_t tick_ms = read_time()*64/(14*1000);

        if ((tick_ms - previous_status) > 3000) {
            // NOTE: 3rd argument to printf not interpolated, just gives 0
            //float sl = 0.0;
            //sl = compute_soundlevel();
            //sl = 33.11f;
            uint64_t sl = compute_soundlevel();
            printf("loop-status time_ms=%d cycles=%d rate=%d sound=%d \n",
                tick_ms, interrupt_cycles, sample_counter/(tick_ms/1000), (int)sl);
            previous_status = tick_ms;
        }
    }

    return 0;
}
