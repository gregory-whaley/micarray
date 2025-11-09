/*
   Pi Pico implementation of a USB interface to stereo MEMS microphones.
   G. Whaley
   22 Oct 2025


*/


#include <stdio.h>
#include "pico/stdlib.h"
#include "stereo_mic_i2s.c"
#include "usb_mic_callbacks.h"

const struct microphone_config mic_config = {
    .gpio_data = 2,                         // GPIO pin for the I2S DAT signal
    .gpio_clk = 3,                          // GPIO pin for the I2S CLK signal
    .pio = pio0,                            // PIO instance to use
    .pio_sm = 0,                            // PIO State Machine instance to use
};

void on_usb_microphone_tx_ready(){                  // Callback from TinyUSB library when all data is ready
                                                        // to be transmitted.
    usb_buffer_ready = true;
}

int main()
{
    uint decimate_factor = 0;

    stdio_init_all();

    printf("Starting...\n");

    i2s_microphone_init(mic_config);
    i2s_microphone_start(mic_config);

    usb_microphone_init();
    usb_microphone_set_tx_ready_handler(on_usb_microphone_tx_ready);


    while (true) {
        while ((sample_buffer_ready == 0) && (usb_buffer_ready == 0)) {     
             // sample_buffer_ready and usb_buffer_ready change in the background and are volatile
            usb_microphone_task();              // spend most time here polling for usb tasks
        }

/*
        if (decimate_factor % 100 == 0) 
            for (int i=0; i< SAMPLE_BUFFER_SIZE; i++){
                printf("%10d    %10d   %10d\n",i,sample_buffer[i][0],sample_buffer[i][1]);
            }
        decimate_factor++;
*/  

        //  we have a i2s buffer full to process    

        usb_microphone_write(sample_buffer, sizeof(sample_buffer));  // Write local buffer to the USB microphone
                    // sample_buffer is array of interleaved 32bit ints.
                    // size is number of bytes.

        sample_buffer_ready = false;
        usb_buffer_ready = false;

    }
};

