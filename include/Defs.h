#pragma once

#include "Configuration.h"
#include <Arduino.h>

// -------------------------------------------------------------------------------

#define PROTO4               // Proto 2 and below has the encoder B and btn3 swapped 

// -------------------------------------------------------------------------------

#define DIGIT_COUNT 6

#define WDT_TIMEOUT 5

#define SERIAL_BAUD_RATE 115200

// Onboard LED 
#define LED_PIN 2

// Digit drivers
#define CLKPin    19
#define BLANKPin  18

#define DATA1Pin  23
#define LATCH1Pin 17

#define DATA2Pin  26
#define LATCH2Pin 27

#define DATA3Pin  32
#define LATCH3Pin 33

// Encoder
#define ENC_APin  5
#define ENC_BPin  14
#define ENC_BTN   16

// Internally defined
#define SDAint    21
#define SCLint    22
#define RX0Pin    3
#define TX0Pin    1

// Touch capable buttons
#define BTN1Pin   15
#define BTN2Pin   4
#define BTN3Pin   12

// Analogue capable - no internal pullups
#define LDRPin    34
#define PIRPin    35

#define PPSPin    0

#define LED_DOUT  13

// -------------------------------------------------------------------------------

#define H10 0
#define H1  1
#define M10 2
#define M1  3
#define S10 4
#define S1  5
