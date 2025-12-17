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
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * The above copyright notice and this permission notice shall be included in
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "usb_mic_callbacks.h"

extern uint8_t const desc_hid_report[];

// Audio controls
// Current states       values to report to host when requested
bool mute; 						// for master channel
uint8_t clkValid;
uint32_t sampFreq;        // sample frequency in Hz

// Range states
audio20_control_range_4_n_t(1) sampleFreqRng; 						// Sample frequency range state

const int muted_buffer[(CFG_TUD_AUDIO_EP_SZ_IN)/4] = {0};     // array of zeros to send if mute is active

void usb_microphone_init() {
  tusb_init();

  sampFreq = SAMPLE_RATE;    // Init values
  clkValid = 1;
  sampleFreqRng.wNumSubRanges = 1;
  sampleFreqRng.subrange[0].bMin = SAMPLE_RATE;
  sampleFreqRng.subrange[0].bMax = SAMPLE_RATE;
  sampleFreqRng.subrange[0].bRes = 0;

  mute = false;
}


void usb_microphone_write(const void * data, uint16_t len)
{
  if (mute) {
    tud_audio_write((uint8_t *)muted_buffer, len);
  }
  else {
    tud_audio_write((uint8_t *)data, len);  // note that len is a byte count.  
  // tusb assumes data is in proper PCM format of number of bytes per sample (1,2,4) 
  // and channel interleaving (e.g. L, R, L, R... in the case of 2 chan stereo)
  }
}



//--------------------------------------------------------------------+
// Application Callback API Implementations
//--------------------------------------------------------------------+


//  The audio class callback procedures 


// Invoked when audio class specific set request received for an EP
bool tud_audio_set_req_ep_cb(uint8_t rhport, tusb_control_request_t const * p_request, uint8_t *pBuff)
{
  (void) rhport;
  (void) pBuff;

  // We do not support any set range requests here, only current value requests
  TU_VERIFY(p_request->bRequest == AUDIO20_CS_REQ_CUR);

  // Page 91 in UAC2 specification
  uint8_t channelNum = TU_U16_LOW(p_request->wValue);
  uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
  uint8_t ep = TU_U16_LOW(p_request->wIndex);

  (void) channelNum; (void) ctrlSel; (void) ep;

  return false; 	// Yet not implemented
}

// Invoked when audio class specific set request received for an interface
bool tud_audio_set_req_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request, uint8_t *pBuff)
{
  (void) rhport;
  (void) pBuff;

  // We do not support any set range requests here, only current value requests
  TU_VERIFY(p_request->bRequest == AUDIO20_CS_REQ_CUR);

  TU_LOG2("        AC specific interface set request\r\n");
  // Page 91 in UAC2 specification
  uint8_t channelNum = TU_U16_LOW(p_request->wValue);
  uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
  uint8_t itf = TU_U16_LOW(p_request->wIndex);

  (void) channelNum; (void) ctrlSel; (void) itf;

  return false; 	// Yet not implemented
}

// Invoked when audio class specific set request received for an entity
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const * p_request, uint8_t *pBuff)
{
  (void) rhport;

  // Page 91 in UAC2 specification
  uint8_t channelNum = TU_U16_LOW(p_request->wValue);
  uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
  uint8_t itf = TU_U16_LOW(p_request->wIndex);
  uint8_t entityID = TU_U16_HIGH(p_request->wIndex);

  (void) itf;

  // We do not support any set range requests here, only current value requests
  TU_VERIFY(p_request->bRequest == AUDIO20_CS_REQ_CUR);

  // If request is for our feature unit
  if ( entityID == 2 )
  {
    switch ( ctrlSel )
    {
      case AUDIO20_FU_CTRL_MUTE:
        // Request uses format layout 1
        TU_VERIFY(p_request->wLength == sizeof(audio20_control_cur_1_t));
        mute = ((audio20_control_cur_1_t*) pBuff)->bCur;
        TU_LOG2("    Set Mute: %d of channel: %u\r\n", mute, channelNum);
      return true;

        // Unknown/Unsupported control
      default:
        TU_BREAKPOINT();
      return false;
    }
  }
  return false;    // Yet not implemented
}

// Invoked when audio class specific get request received for an EP
bool tud_audio_get_req_ep_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
  (void) rhport;

  // Page 91 in UAC2 specification
  uint8_t channelNum = TU_U16_LOW(p_request->wValue);
  uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
  uint8_t ep = TU_U16_LOW(p_request->wIndex);
  TU_LOG2("    AC specific get request for EP: %d",ep);

  (void) channelNum; (void) ctrlSel; (void) ep;
//	return tud_control_xfer(rhport, p_request, &tmp, 1);

  return false; 	// Yet not implemented
}

// Invoked when audio class specific get request received for an interface
bool tud_audio_get_req_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
  (void) rhport;
  TU_LOG2("Interface requested ");
  // Page 91 in UAC2 specification
  uint8_t channelNum = TU_U16_LOW(p_request->wValue);
  uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
  uint8_t itf = TU_U16_LOW(p_request->wIndex);

  TU_LOG2("Interface %d requested ",itf);
  (void) channelNum; (void) ctrlSel; (void) itf;

  return false; 	// Yet not implemented
}

// Invoked when audio class specific get request received for an entity
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
  (void) rhport;

  // Page 91 in UAC2 specification
  uint8_t channelNum = TU_U16_LOW(p_request->wValue);
  uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
  // uint8_t itf = TU_U16_LOW(p_request->wIndex); 			// Since we have only one audio function implemented, we do not need the itf value
  uint8_t entityID = TU_U16_HIGH(p_request->wIndex);

  // Input terminal (Microphone input)
  if (entityID == 1)
  {
    switch (ctrlSel)
    {
      case AUDIO20_TE_CTRL_CONNECTOR: {
      // The terminal connector control only has a get request with only the CUR attribute.

      audio20_desc_channel_cluster_t ret;

      // Those are dummy values for now
      ret.bNrChannels = 2;
      ret.bmChannelConfig = 0;
      ret.iChannelNames = 0;

      TU_LOG2("    Get terminal connector\r\n");

      return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, (void*)&ret, sizeof(ret));
      } break;

      // Unknown/Unsupported control selector
      default: TU_BREAKPOINT(); return false;
    }
  }

  // Feature unit
  if (entityID == 2)
  {
    switch (ctrlSel)
    {
      case AUDIO20_FU_CTRL_MUTE:
	// Audio control mute cur parameter block consists of only one byte - we thus can send it right away
	// There does not exist a range parameter block for mute
      	TU_LOG2("    Get Mute of channel: %u\r\n", channelNum);
	      return tud_control_xfer(rhport, p_request, &mute, 1);

// Unknown/Unsupported control
	  default: TU_BREAKPOINT(); return false;
    }
  }

  // Clock Source unit
  if (entityID == 4)
  {
    switch (ctrlSel)                              // which control was selected?
    {
      case AUDIO20_CS_CTRL_SAM_FREQ:                //  the sample frequency was asked for

	// channelNum is always zero in this case

      switch (p_request->bRequest)
      { 
        case AUDIO20_CS_REQ_CUR:                    //  requested the current value
          TU_LOG2("    Get Sample Freq. %d\r\n",sampFreq);
          return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &sampFreq, sizeof(sampFreq));
        case AUDIO20_CS_REQ_RANGE:                  //  requested the range of allowed values
          TU_LOG2("    Get Sample Freq. range\r\n");
          return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &sampleFreqRng, sizeof(sampleFreqRng));

          // Unknown/Unsupported control
        default: TU_BREAKPOINT(); return false;
      }

      case AUDIO20_CS_CTRL_CLK_VALID:
        // Only cur attribute exists for this request
        TU_LOG2("    Get Sample Freq. valid\r\n");
        return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &clkValid, sizeof(clkValid));

        // Unknown/Unsupported control
      default: TU_BREAKPOINT(); return false;
    }
  }

  TU_LOG2("  Unsupported entity: %d\r\n", entityID);
  return false; 	// Yet not implemented
}


bool tud_audio_tx_done_post_load_cb(uint8_t rhport, uint16_t n_bytes_copied, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting)
{
  (void) rhport;
  (void) n_bytes_copied;
  (void) itf;
  (void) ep_in;
  (void) cur_alt_setting;

  return true;
}

bool tud_audio_set_itf_close_EP_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
  (void) rhport;
  (void) p_request;

  return true;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+
//  The HID class-specific callback procedures



// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_hid_descriptor_report_cb(uint8_t itf)
{
  (void) itf;
  return desc_hid_report;
}


int16_t read_temperature() {
  return (int16_t)100.0*(27.0-((adc_read()*3.28/4096.0)-0.706)*581.0);          // convert adc reading to degrees C * 100
}

const int16_t mic_dist_mm = 400;                                              // returns the mic spacing in mm


// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  int16_t response;

  if (report_id == 1) {                   //  report ID 1 is the only supported report
    response =  read_temperature();
    *(buffer) = (char)(response & 0xFF);    // write the byte portions to the buffer pointer location
    *(buffer+1) = (char)(response >> 8);    //  LSB first, then MSB
    *(buffer+2) = (char)(mic_dist_mm & 0xFF);
    *(buffer+3) = (char)(mic_dist_mm >> 8);
  }

  (void) itf;
  (void) report_type;
  (void) reqlen;
  return 4;                                 // 4 bytes of data copied to buffer
}


// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;

}


