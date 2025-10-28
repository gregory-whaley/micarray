/*
   Pi Pico implementation of a USB interface to stereo MEMS microphones.
   G. Whaley
   22 Oct 2025


*/

#include <stdio.h>

#include "pico/stdlib.h"
#include "stereo_mic_i2s.c"

const struct microphone_config mic_config = {
    .gpio_data = 2,                         // GPIO pin for the I2S DAT signal
    .gpio_clk = 3,                          // GPIO pin for the I2S CLK signal
    .pio = pio0,                            // PIO instance to use
    .pio_sm = 0,                            // PIO State Machine instance to use
    .sample_rate = 48000,                   //  fixed sample rate for this microphone in Hz
    .sample_buffer_size = 256,              //  sample buffer is two column array of signed 32 bit ints.
};



int main()
{
    stdio_init_all();


    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
};
