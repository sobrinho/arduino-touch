/*
  Touch.cpp

  Copyright (c) 2021, Gabriel Sobrinho
  Original code (pre-library): Copyright (c) 2015, Arduino LLC
  Original code (pre-library): Copyright (c) 2011, Peter Barrett

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Touch.h"

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)

#if defined(_USING_HID)

static const uint8_t _hidReportDescriptor[] PROGMEM = {
  
    //  Touch
    0x05, 0x0d,                    // USAGE_PAGE (Digitizer)
    0x09, 0x02,                    // USAGE (Pen)
    0xa1, 0x01,                    // COLLECTION (Application)
    	
    // declare a finger collection
    0x09, 0x20,                    //   Usage (Stylus)
    0xA1, 0x00,                    //   Collection (Physical)

    // Declare a finger touch (finger up/down)
    0x09, 0x42,                    //     Usage (Tip Switch)
    0x09, 0x32,                    //     USAGE (In Range)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)

    // Declare the remaining 6 bits of the first data byte as constant -> the driver will ignore them
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x06,                    //     REPORT_COUNT (6)
    0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)

    // Define absolute X and Y coordinates of 16 bit each (percent values multiplied with 100)
    // http://www.usb.org/developers/hidpage/Hut1_12v2.pdf
    // Chapter 16.2 says: "In the Stylus collection a Pointer physical collection will contain the axes reported by the stylus."
    0x05, 0x01,                    //     Usage Page (Generic Desktop)
    0x09, 0x01,                    //     Usage (Pointer)
    0xA1, 0x00,                    //     Collection (Physical)
    0x09, 0x30,                    //        Usage (X)
    0x09, 0x31,                    //        Usage (Y)
    0x16, 0x00, 0x00,              //        Logical Minimum (0)
    0x26, 0x10, 0x27,              //        Logical Maximum (10000)
    0x36, 0x00, 0x00,              //        Physical Minimum (0)
    0x46, 0x10, 0x27,              //        Physical Maximum (10000)
    0x66, 0x00, 0x00,              //        UNIT (None)
    0x75, 0x10,                    //        Report Size (16),
    0x95, 0x02,                    //        Report Count (2),
    0x81, 0x02,                    //        Input (Data,Var,Abs)
    0xc0,                          //     END_COLLECTION

    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION

    // With this declaration a data packet must be sent as:
    // byte 1   -> "touch" state          (bit 0 = pen up/down, bit 1 = In Range)
    // byte 2,3 -> absolute X coordinate  (0...10000)
    // byte 4,5 -> absolute Y coordinate  (0...10000)
};

//================================================================================
//================================================================================
//	Touch

Touch_::Touch_(void)
{
    static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor));
    HID().AppendDescriptor(&node);
}

void Touch_::begin(void)
{
}

void Touch_::end(void)
{
}

void Touch_::touch(uint16_t x, uint16_t y)
{
	uint16_t m[5];
	m[0] = 1;
	m[1] = LSB(x);
	m[2] = MSB(x);
	m[3] = LSB(y);
	m[4] = MSB(y);
	HID().SendReport(1,m,5);
}

Touch_ Touch;

#endif
