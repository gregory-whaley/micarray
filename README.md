# Micarray
Multichannel USB microphone for Raspbery Pi Pico for use in acoustic array beamforming.

This C code and PIO code will accept I2S data from two MEMS microphones and create a USB 2.0 audio microphone source with two channels intially, and expandable to more channels in later versions.  The target MCU is the Raspberry Pi RP2040 on the Pico board and the target microphone is the Invensense ICS-43434 MEMS microphone with I2S interface.  The code supports the following features:

- USB 2.0 device enumerates as a standard audio class 2.0 isochronous streaming device.  No host driver installation is necessary for Windows/Mac/Linux.
- Audio sample rate is fixed at 48000 Hz.
- Data is encoded as PCM samples SE_32 (32 bits per sample).
- Audio volume (gain) is fixed and so volume control must be performed at the host application level.
- MEMS microphone interface is I2S with both outputs interleaved into one data path.
- Target MEMS microphone is Invensense ICS-43434, 24 bits/sample.
- Uses Raspberry Pi Pico RP2040 development board.
- USB enumeration of HID interface providing MCU (environment) temperature which is important for acoustic beamforming.


## Documentation



### Hardware
The system can be constructed using a Raspberry Pi Pico development board and two MEMS microphone evaluation boards from Adafruit (Part #6049) as shown in the following diagram.
![Connection Diagram for Dual Microphones](./pico_diagram.png)  Connection Diagram for Dual Microphones


### Supported Boards
- Raspberry Pi Pico/W (RP2040)
- Adafruit #6049 MEMS microphone evaluation board

### Building and Installing

The code is built using the standard [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk) and leverages the [TinyUSB library](https://github.com/hathach/tinyusb) for usb functions.

Instructions for installing the Pico development SDK [can be found here](https://rptl.io/pico-get-started).

Clone this repo to a new folder in your local system.
```shell
mkdir micarray
git clone https://github.com/gregory-whaley/micarray.git
```

Unfortunately the version of TinyUSB which is automatically included with the Pico SDK has a buffer allocation error in streaming audio.  This error has been fixed in version 0.19.0, so we must instead clone the current TinyUSB repository also to a separate folder in your local system.
```shell
mkdir tinyusb
git clone https://github.com/hathach/tinyusb.git
```


  The CMakeLists.txt file in the micarray repo includes a redirection to this updated local TinyUSB folder, and so you must edit the following line to point cmake to your local TinyUSB folder.
```
set(PICO_TINYUSB_PATH "/home/YOUR_LOCAL_FOLDER/tinyusb")
```

The code is built from the linux command line as follows:
```shell
cd YOUR_LOCAL_FOLDER/micarray
mkdir build
cd build
cmake .. -DBOARD=raspberry_pi_pico
make
```
which will produce a *.uf2 binary output file in the build folder.  Then follow the standard process to flash the file to the pico by plugging in the pico with the bool_sel button pressed, and copy the uf2 file to the pico folder which is mounted to the system.  After flashing, the pico will present both a standard audio streaming USB interface, and an HID interface.  The audio function can be tested using any recording application such as Audacity.  The hid_test.py script can be used to query the HID functions which returns the pico device temperature and a (hard coded) number representing the physical distance between microphones in the array.  In linux HID devices are owned by root by default and thus blocked from user access, so the simplest method to run the python script is to run as root.


## License
The MIT License (MIT)

Copyright (C) 2025 The Whaley Group

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 