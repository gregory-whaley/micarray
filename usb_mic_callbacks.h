/*
 * 
 */

#ifndef _USB_MIC_CALLBACKS_H_
#define _USB_MIC_CALLBACKS_H_

#include "tusb_config.h"
#include "tusb.h"

#ifndef SAMPLE_RATE
#define SAMPLE_RATE ((CFG_TUD_AUDIO_EP_SZ_IN / 8) - 1) * 1000
#endif

#ifndef SAMPLE_BUFFER_SIZE
#define SAMPLE_BUFFER_SIZE ((CFG_TUD_AUDIO_EP_SZ_IN/2) - 1)
#endif


#include "hardware/adc.h"

void usb_microphone_init();
void usb_microphone_write(const void * data, uint16_t len);

#endif
