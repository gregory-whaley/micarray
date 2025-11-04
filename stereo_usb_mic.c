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
};



int main()
{
    uint decimate_factor = 0;

    stdio_init_all();

    printf("Starting...\n");

    i2s_microphone_init(mic_config);
    i2s_microphone_start(mic_config);

    while (true) {
        while (sample_buffer_ready == 0) { tight_loop_contents(); }            // sample_buffer_ready changes in the background and is volatile

        if (decimate_factor % 100 == 0) 
            for (int i=0; i< SAMPLE_BUFFER_SIZE; i++){
                printf("%10d    %10d   %10d\n",i,sample_buffer[i][0],sample_buffer[i][1]);
            }
        decimate_factor++;
        sample_buffer_ready = false;

    }
};
