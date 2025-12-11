#!/usr/bin/env python3
# Install python3 HID package https://pypi.org/project/hid/
import hid
import time


# default is TinyUSB (0xcafe), Adafruit (0x239a), RaspberryPi (0x2e8a), Espressif (0x303a) VID
USB_VID = (0xcafe, 0x239a, 0x2e8a, 0x303a)
report_ID = 1

print("VID list: " + ", ".join('%02x' % v for v in USB_VID))

for vid in  USB_VID:
    for dict in hid.enumerate(vid):
        print(dict)
        dev = hid.Device(dict['vendor_id'], dict['product_id'])
        if dev:
            while True:
                report = dev.get_feature_report(report_ID,5)   # first byte is reportID, then data bytes LSB first
                print("Bytes: ",report[0],report[1],report[2],report[3],report[4],"  Temp:",report[2]*256+report[1],"  Dist:",report[4]*256+report[3])      
                time.sleep(1)
