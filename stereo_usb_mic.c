/*
 * The MIT License (MIT)
 *
 * Copyright (C) 2025 The Whaley Group
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *
 *


    Pi Pico implementation of a USB 2.0 interface to multiple MEMS microphones.
    G. Whaley
    22 Oct 2025

    Functions as usb 2.0 interface to two or more MEMS microphones for the
    purpose of acoustic beamforming applications.

    Functional Specifications:
    - USB 2.0 device enumerates as a standard audio class 2.0 device.
    - Audio sample rate is fixed at 48000 Hz.
    - Data is encoded as PCM samples SE_32 (32 bits per sample).
    - MEMS microphones output I2S data interface.
    - Target MEMS microphone is Invensense ICS-43434, 24 bits/sample
    - Uses Raspberry Pi Pico RP2040 microcontroller.
    - USB enumeration of HID interface providing MCU temperature.

    Codebase uses the TinyUSB library.  Note the version of Tinyusb supplied
    with the Pico SDK has an endpoint buffer bug, which has been fixed in
    Tinyusb version 0.19.0 and later so this code does not reference the
    SDK version.
*/


#include <stdio.h>
#include "pico/stdlib.h"
#include "stereo_mic_i2s.c"
#include "usb_mic_callbacks.h"
#include "hardware/adc.h"
#include "bsp/board_api.h"


const struct microphone_config mic_config = {
    .gpio_data = 2,                         // GPIO pin for the I2S DAT signal
    .gpio_clk = 3,                          // GPIO pin for the I2S CLK signal
    .pio = pio0,                            // PIO instance to use
    .pio_sm = 0,                            // PIO State Machine instance to use
};

int decimate = 0;


int main()
{
    stdio_init_all();                                   //  supports standard uart output for printf.
    i2s_microphone_init(mic_config);
    i2s_microphone_start(mic_config);
    usb_microphone_init();                              // contains tusb_init()

    adc_init();
    adc_select_input(4);                                //  chan 4 is the internal temperature sensor
    adc_set_temp_sensor_enabled(true);                  // activate the internal pullup resistor to bias temp sensor
    board_init();
    board_led_write(1);                                 // turn on LED for USB power indicator


    while (true) {
        while (sample_buffer_ready == 0) {     // sample_buffer_ready changes in the background and is volatile
            tud_task();                         // spend most time here polling for usb tasks
        }  
                                                                    //  at this point we have a i2s buffer full to process    
        usb_microphone_write(sample_buffer, sizeof(sample_buffer));  // Write local buffer to the USB microphone
                                                                    // sample_buffer is array of interleaved 32bit ints.
                                                                    // size is number of bytes.
        sample_buffer_ready = false;
    }
};

