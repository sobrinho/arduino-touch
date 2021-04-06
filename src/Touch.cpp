#include "Touch.h"

#if defined(_USING_HID)

#define CONTACT_COUNT_MAXIMUM 1
#define REPORTID_TOUCH        0x01

#define LSB(v) ((v >> 8) & 0xff)
#define MSB(v) (v & 0xff)

static const uint8_t _hidReportDescriptor[] PROGMEM = {
  0x05, 0x0d,                    /* USAGE_PAGE (Digitizer) */
  // 0x09, 0x02,                 /* USAGE (Pen) */
  0x09, 0x04,                    /* USAGE (Touch Screen) */
  0xa1, 0x01,                    /* COLLECTION (Application) */
  0x85, REPORTID_TOUCH,          /*    REPORT_ID */

  /* declare a finger collection */
  0x09, 0x20,                    /*   Usage (Stylus) */
  0xA1, 0x00,                    /*   Collection (Physical) */

  /* Declare a finger touch (finger up/down) */
  0x09, 0x42,                    /*     Usage (Tip Switch) */
  0x09, 0x32,                    /*     USAGE (In Range) */
  0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */
  0x25, 0x01,                    /*     LOGICAL_MAXIMUM (1) */
  0x75, 0x01,                    /*     REPORT_SIZE (1) */
  0x95, 0x02,                    /*     REPORT_COUNT (2) */
  0x81, 0x02,                    /*     INPUT (Data,Var,Abs) */

  /* Declare the remaining 6 bits of the first data byte as constant -> the driver will ignore them */
  0x75, 0x01,                    /*     REPORT_SIZE (1) */
  0x95, 0x06,                    /*     REPORT_COUNT (6) */
  0x81, 0x01,                    /*     INPUT (Cnst,Ary,Abs) */

  /* Define absolute X and Y coordinates of 16 bit each (percent values multiplied with 100) */
  /* http://www.usb.org/developers/hidpage/Hut1_12v2.pdf */
  /* Chapter 16.2 says: "In the Stylus collection a Pointer physical collection will contain the axes reported by the stylus." */
  0x05, 0x01,                    /*     Usage Page (Generic Desktop) */
  0x09, 0x01,                    /*     Usage (Pointer) */
  0xA1, 0x00,                    /*     Collection (Physical) */
  0x09, 0x30,                    /*        Usage (X) */
  0x09, 0x31,                    /*        Usage (Y) */
  0x16, 0x00, 0x00,              /*        Logical Minimum (0) */
  0x26, 0x10, 0x27,              /*        Logical Maximum (10000) */
  0x36, 0x00, 0x00,              /*        Physical Minimum (0) */
  0x46, 0x10, 0x27,              /*        Physical Maximum (10000) */
  0x66, 0x00, 0x00,              /*        UNIT (None) */
  0x75, 0x10,                    /*        Report Size (16), */
  0x95, 0x02,                    /*        Report Count (2), */
  0x81, 0x02,                    /*        Input (Data,Var,Abs) */
  0xc0,                          /*     END_COLLECTION */
  0xc0,                          /*   END_COLLECTION */
  0xc0                           /* END_COLLECTION */

  // With this declaration a data packet must be sent as:
  // byte 1   -> "touch" state          (bit 0 = pen up/down, bit 1 = In Range)
  // byte 2,3 -> absolute X coordinate  (0...10000)
  // byte 4,5 -> absolute Y coordinate  (0...10000)
};


typedef struct Finger {
  int contact;
  int16_t x;
  int16_t y;
} Finger;

Finger fingers[CONTACT_COUNT_MAXIMUM];


Touch_::Touch_() {
  static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor));
  HID().AppendDescriptor(&node);

  for (int i = 0; i < CONTACT_COUNT_MAXIMUM; i++) {
    fingers[i].contact = 0;
    fingers[i].x = 0;
    fingers[i].y = 0;
  }
}

void Touch_::begin(void) {
}

void Touch_::end(void) {
}


void Touch_::send(uint8_t identifier, uint8_t touch, int16_t x, int16_t y) {
  // calculate current contact count
  uint8_t contact = 0;
  
  for (int i = 0; i < CONTACT_COUNT_MAXIMUM; i++) {
    if (fingers[i].contact) {
      contact += 1;
    }
  }

  // create data array
  uint8_t data[] = {
    touch,             // finger touches display
    MSB(x), LSB(x),    // x
    MSB(y), LSB(y)     // y
  };

  // send packet
  HID().SendReport(REPORTID_TOUCH, data, 5);
}


void Touch_::moveFingerTo(uint8_t identifier, int16_t x, int16_t y) {
  // change finger record
  fingers[identifier].contact = 1;
  fingers[identifier].x = x;
  fingers[identifier].y = y;

  // send update
  send(identifier, 3, x, y);
}

void Touch_::releaseFinger(uint8_t identifier) {
  // change finger record
  fingers[identifier].contact = 0;
  fingers[identifier].x = 0;
  fingers[identifier].y = 0;

  // send update
  send(identifier, 0, 0, 0);
}

Touch_ Touch;

#endif
