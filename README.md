# Micarray
Multichannel USB microphone for Raspbery Pi Pico for use in acoustic array beamforming.

C code and PIO code to accept I2S data from MEMS microphones and create a USB 2.0 audio microphone source with two channels intially and expandable to more channels.  The target MCU is the Raspberry Pi RP2040 on the Pico board and the target microphone is the Invensense ICS-43434 MEMS microphone with I2S interface.  The code supports the following features:

- USB 2.0 device enumerates as a standard audio class 2.0 device.  No host driver installation is necessary.
- Audio sample rate is fixed at 48000 Hz.
- Data is encoded as PCM samples SE_32 (32 bits per sample).
- MEMS microphones interface is I2S.
- Target MEMS microphone is Invensense ICS-43434, 24 bits/sample.
- Uses Raspberry Pi Pico RP2040 development board.
- USB enumeration of HID interface providing MCU (environment) temperature which is important for acoustic beamforming.


## Documentation

The code is built using the standard [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk) and leverages the [TinyUSB library](https://github.com/hathach/tinyusb) for usb functions.

### Hardware

### Supported Boards
- Raspberry Pi Pico/W (RP2040)


### Building and Installing

To do

## License
The MIT License (MIT)

Copyright (C) 2025 The Whaley Group

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 