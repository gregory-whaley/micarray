/*
   Pi Pico implementation of a USB interface to stereo MEMS microphones.
   G. Whaley
   22 Oct 2025


*/

#include <stdio.h>

#include "stereo_usb_mic.pio.h"       // include the compiled pio program data

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"

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

The channel is started by writing to the channel trigger register CTRL_TRIG.

There is an enable/disable bit in CTRL.EN

When a dma transfer has filled the buffer, an interrupt can be generated if:
CTRL.IRQ_QUIET is disabled, and
The INTS register holds status for all possible interrupts, the dma irq being only
one of them.  So the status bit can be masked to test for an interrupt status
and the bit written to clear the status once the service routine is called.

There are two independent interrupt systems, 0 and 1.

After a dma transfer is completed and an interrupt is triggered, the dma
write address and count need updating to a new buffer location and the dma
triggered again.

Sequence of calls:
    dma_chan = dma_claim_unused_channel(true);                      // get the dma channel
    dma_channel_config c = dma_channel_get_default_config(dma_chan);            // obtain all the channel defaults
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);                     // set the transfer size
    channel_config_set_read_increment(&c, false);                               // set for no read increment
    channel_config_set_dreq(&c, DREQ_PIO0_RX0);                                 // set for DREQ pacing on the input FIFO
    dma_channel_set_irq0_enabled(dma_chan, true);                               // set the dma complete to call irq0
    irq_set_exclusive_handler(DMA_IRQ_0, my_dma_handler);                       // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_enabled(DMA_IRQ_0, true);
    dma_channel_configure();                                                    // call to configure the hardware with the c struct parameters but don't yet start the dma
    my_dma_handler();                                                           // Manually call the interrupt handler once, to trigger the first transfer

The interrupt handler will:
    clear the int status bit
    reload the dma write and transfer count for a new empty buffer and re-trigger the dma
    tell the consumer that a full buffer is ready to read.

*/




/*
Notes for main() routine activation of hardware:
    claim a PIO, 0 or 1
    claim a state machine 0-3
    call the init helper routine from the .pio file
    set the sm clock divider
    initialize the dma buffers and initialize the dma hardware to run
    enable the interrupt hardware
    pio_sm_set_enabled(pio, sm, true);                      // enable the state machine to start executing.
*/



int main()
{
    stdio_init_all();

    PIO pio = pio0;

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
