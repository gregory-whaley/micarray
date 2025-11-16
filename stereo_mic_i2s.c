/*
C routines to support pico I2S interface to Invensense ICS-43434 MEMS microphone.
G. Whaley
28 Oct 25


*/

#include <stdio.h>
#include "pico/stdlib.h"              // declares a uint among other things
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "stereo_mic_i2s.pio.h"       // include the compiled pio program data, this line must follow the above #includes

#define I2S_SAMPLE_BUFFER_SIZE 48               // number of stored samples of each channel
#define I2S_SAMPLE_RATE 48000                   //  fixed sample rate for this microphone in Hz

struct microphone_config {                 // struct to contain hardware choices for connecting the I2S interface
    uint gpio_data;                         // GPIO pin for the I2S DAT signal
    uint gpio_clk;                          // GPIO pin for the I2S CLK signal
    PIO  pio;                                // PIO instance to use
    uint pio_sm;                            // PIO State Machine instance to use
};


/*
Notes for dma implementation:

There are 12 independent dma channels, I will pick one for ISR FIFO to memory.
Each channel has 4 configuration registers: The current read address, the current
write address, the word transfer count, and the control register.
The read address will be fixed on the input FIFO so the address will be set
and not automatically incremented.  The write address will be set via both
software control to init the two buffer memory locations, and the dma
will increment the write address automatically.
The software will initialize the trans_count register for writing.
CTRL.DATA_SIZE will be 4 bytes per word
CTRL.INCR_WRITE will be true.
CTRL.INCR_READ will be false (i.e. fixed on the FIFO)
The CTRL.TREQ_SEL will be set to trigger dma transfer when the FIFO needs
servicing.  So e.g. DREQ_PIO0_RX0 sets the PIO=0, state_machine=0 input FIFO
as the DREQ trigger.

The channel is started by writing to the channel trigger register CTRL_TRIG.  This happens in dma_channel_transfer_to_buffer_now()

There is an enable/disable bit in CTRL.EN which is activated in dma_channel_configure()

When a dma transfer has filled the buffer, an interrupt can be generated if:
CTRL.IRQ_QUIET is disabled, and
The INTS register holds status for all possible interrupts, the dma irq being only
one of them.  So the status bit can be masked to test for an interrupt status
and the bit written to clear the status once the service routine is called.

After a dma transfer is completed and an interrupt is triggered, the dma
write address and count need updating to a new buffer location and the dma
triggered again.

*/

int sample_buffer[I2S_SAMPLE_BUFFER_SIZE][2];                                       // left channel is index 0, right is index 1
volatile uint sample_buffer_ready = false;
volatile int raw_dma_buffer[I2S_SAMPLE_BUFFER_SIZE*2];                                           // storage for interleaved FIFO data
int dma_chan;

// routine to manage the two raw data buffers into which the dma will copy raw data from the FIFO.
// it is set to be called when the irq0 is triggered upon the dma transfer complete event.
void my_dma_handler(){
    dma_hw->ints0 = (1u << dma_chan);                   // ack the interrupt by writing a mask to the status register

    for(int i = 0; i < I2S_SAMPLE_BUFFER_SIZE; i++){          // copy interleaved raw data to sample buffer while shifting 8 bits
        sample_buffer[i][0] = raw_dma_buffer[i*2];
        sample_buffer[i][1] = raw_dma_buffer[i*2+1];
    }
    sample_buffer_ready = true;
    dma_channel_transfer_to_buffer_now(dma_chan,raw_dma_buffer,I2S_SAMPLE_BUFFER_SIZE*2);   // set the dma to transfer another block, assume buffer is empty
};


void i2s_microphone_init(struct microphone_config config ) {

    uint pio_sm_offset = pio_add_program(config.pio, &i2s_mic_program);            // installs the pio code and returns its offset location

    // init the pio with the helper function defined in the pio file
    i2s_mic_program_init(config.pio, config.pio_sm, pio_sm_offset, config.gpio_data, config.gpio_clk);

    //  set up the dma system to manage data arriving from the FIFO
    dma_chan = dma_claim_unused_channel(true);                              // get the dma channel number
    dma_channel_config c = dma_channel_get_default_config(dma_chan);            // obtain all the channel default parameters
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);                     // set the transfer size = 32 bits (4 bytes)
    channel_config_set_read_increment(&c, false);                               // set for no read increment
    channel_config_set_write_increment(&c,true);                                // set for write address increment after each write to dma memory
    channel_config_set_dreq(&c, DREQ_PIO0_RX0);                                 // set for DREQ pacing on the input FIFO
    dma_channel_configure(dma_chan, &c, &raw_dma_buffer,
         &config.pio->rxf[config.pio_sm], I2S_SAMPLE_BUFFER_SIZE*2, false);  // take the dma config parms and load them into hardware, false=don't start dma yet

    dma_channel_set_irq0_enabled(dma_chan, true);                               // set the dma complete to call irq0
    irq_set_exclusive_handler(DMA_IRQ_0, my_dma_handler);                       // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_enabled(DMA_IRQ_0, true);                                           // turn on the irq hardware
};

void i2s_microphone_start(struct microphone_config config) {
    //  launches the hardware running with an initially empty buffer
    dma_channel_transfer_to_buffer_now(dma_chan,raw_dma_buffer,I2S_SAMPLE_BUFFER_SIZE*2);   // set the dma to transfer another block, assume buffer is empty
    dma_channel_start(dma_chan);          //  enable the dma hardware enable bit.
    pio_sm_set_enabled(config.pio,config.pio_sm,true);
};