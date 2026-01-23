#pragma once

#include <ESPAsyncWebServer.h>
#include "WiFi.h"
#include "Defs.h"
#include "esp_wps.h"

#include "SpiffsStorage.h"          // Access to config objects
#include "StorageTypes.h"           // Config and Stats objects
#include "TubeTypes.h"              // Tube types

// ************************************************************
// Global shared components and objects
// ************************************************************
extern esp_wps_config_t wps_config;

extern unsigned long nowMillis;
extern unsigned long lastSecondStartMillis;
extern int lastSecond;
extern boolean triggeredThisSec;
extern int secsDeltaAbs;
extern int secsDelta;
extern bool upOrDown;  

extern bool blanked;  

// ************************************************************
// Shared config objects
// ************************************************************
extern spiffs_config_t* cc;
extern spiffs_stats_t* cs;

// ************************************************************
// Display values
// ************************************************************
extern volatile uint32_t dispVal;
extern volatile uint8_t dispBoardCount;

// ************************************************************
// Interrupt mutexes
// ************************************************************
extern portMUX_TYPE timerMux1;
extern portMUX_TYPE encoderMux;

// ************************************************************
// General values
// ************************************************************
extern bool doAutoReconnect;

extern String uniqHostname;

extern AsyncWebServer server;

extern String lastWiFiScan;

// Structure for button state
struct Button {
    uint8_t    pin;
    uint32_t   lastDebounceTime = 0;
    bool       lastState = HIGH;
    bool       currentState = HIGH;
    uint32_t   pressedTime = 0;
    uint32_t   releasedTime = 0;
    uint8_t    clickCount = 0;
  
    // Constructor to initialize all members
    Button(gpio_num_t p, uint32_t debounceTime, bool lastSt, bool currentSt, uint32_t pressedT, uint32_t releasedT, uint8_t clickC)
        : pin(p), lastDebounceTime(debounceTime), lastState(lastSt), currentState(currentSt), pressedTime(pressedT), releasedTime(releasedT), clickCount(clickC) {}
  };

extern bool lastExpired;

// ------------------ Global functions -----------------

void updateNowMillis();
