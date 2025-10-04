#include "Defs.h"
#include "Globals.h"
#include "utilities.h"
#include "WiFi.h"
#include "TimerManager.h"
#include "DebugManager.h"
#include "WiFiManager.h"
#include "OutputManager.h"
#include "CounterManager.h"
#ifdef FEATURE_MENU
#include "MenuManager.h"
#endif

// ************************************************************
// Switch changed - mark that there is an event waiting
// ************************************************************
void IRAM_ATTR btn1ISR() {
  btn1ReadMillis = millis() + 50;
}

// ************************************************************
// Switch changed - mark that there is an event waiting
// ************************************************************
void IRAM_ATTR btn2ISR() {
  btn2ReadMillis = millis() + 50;
}

// ************************************************************
// Sset up the unit
// ************************************************************
void setup() {
  // Show that we booted - useful for remote debugging
  pinMode(LED_PIN, OUTPUT);
  pinMode(IND1Pin, OUTPUT);
  pinMode(IND2Pin, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  playTune();
  
  for (int i = 0; i < 10 ; i++) {
    digitalWrite(LED_PIN, (i % 2) == 0);
    digitalWrite(IND1Pin, (i % 2) == 0);   
    digitalWrite(IND2Pin, (i % 2) == 0);   
    delay(50);   
  }

  Serial.begin(SERIAL_BAUD_RATE);

  #ifdef DEBUG
  // Debug for 10 minutes
  debugManager.setDebugAutoOff(600);
  #endif

  // -------------------------------------------------------------------------

  debugMsgMain("Start up GPIOs");
  pinMode(CLKPin, OUTPUT);
  pinMode(DATAPin, OUTPUT);
  pinMode(LATCHPin, OUTPUT);
  pinMode(BLANKPin, OUTPUT);

  // for (int i = 0; i < 200; i++) {
  //   bool state = (i % 2) == 0;
  //   digitalWrite(DATAPin, state);
  //   digitalWrite(CLKPin, state);
  //   digitalWrite(LATCHPin, state);
  //   digitalWrite(BLANKPin, state);
  //   delay(10);
  // }

  pinMode(BTN1Pin, INPUT_PULLUP);
  pinMode(BTN2Pin, INPUT_PULLUP);

  // Hook up the switches to the trigger handler
  attachInterrupt(BTN1Pin, btn1ISR, CHANGE);
  attachInterrupt(BTN2Pin, btn2ISR, CHANGE);

  // -------------------------------------------------------------------------

  nowMillis = millis();

  // -------------------------------------------------------------------------
  
  debugMsgMain("Start up SPIFFS");

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    debugMsgMain("An Error has occurred while mounting SPIFFS");
    return;
  }

  bool statsLoaded = spiffsStorage.getStatsFromSpiffs();

  if (!statsLoaded) {
    debugMsgMain("SPIFFS storage: read stats failed");
    spiffsStorage.saveStatsToSpiffs();
  }

  bool configloaded = spiffsStorage.getConfigFromSpiffs();

  if (configloaded) {
    debugMsgMain("SPIFFS storage: Loaded");
  } else {
    debugMsgMain("SPIFFS storage: read config failed - do factory reset");
    resetOptions();
    spiffsStorage.saveConfigToSpiffs();
  }

  // -------------------------------------------------------------------------

  debugMsgMain("Start up Timers");

  // Starts the display and the status LED flashing
  startTimers();

  // -------------------------------------------------------------------------
  // First start digit test
  debugMsgMain("Startup digit test");
  outputManager.setBlanked(false);

  for (int i = 0 ; i <= 2*MAX_CATHODE_COUNT ; i++) {
    outputManager.loadNumberIntegerValue(i);
    outputManager.outputDisplay();
    delay(100);
  }

  // // -------------------------------------------------------------------------
  // // First start bit test
  // debugMsgMain("Startup bit test");
  // for (int i = 0 ; i < 32 ; i++) {
  //   dispVal = (1 << i);
  //   delay(100);
  // }

  // -------------------------------------------------------------------------
  
  // Default pins SDA 21, SCL 22 Frequency 400kHz 
  debugMsgMain("Start up I2C...");
  Wire.begin(SDAint, SCLint, 400000L);

  // -------------------------------------------------------------------------
  
  #ifdef FEATURE_MENU
  debugMsgMain("Starting OLED");
  oled.setUp();
  oled.clearDisplay();
  menuManager.flashMenuMessage(CLOCK_MENU_TITLE, "Starting");
  #endif

  // -------------------------------------------------------------------------
  
  debugMsgMain("Initialising WiFi");
  wifiManager.setUpWiFi();

  if (cc->WifiOnAtStart && wifiManager.wifiCredentialsReceived()) {
    debugMsgMain("Connecting to previous AP");    
    wifiManager.connectToLastAP();
  } else {
    if (!cc->WifiOnAtStart) {
      debugMsgMain("Skipping connect to previous AP - told not to");
    } else if (!wifiManager.wifiCredentialsReceived()) {
      debugMsgMain("Skipping connect to previous AP - no AP defined");
    }
  }

  // -------------------------------------------------------------------------

  #ifdef FEATURE_MENU
  debugMsgMain("Start up Menu Manager...");
  menuManager.setupMenuManager();
  oled.setUp();
  oled.clearDisplay();
  menuManager.flashMenuMessage(CLOCK_MENU_TITLE, "Welcome to the\nNixie Burner\n" + String(SOFTWARE_VERSION));
  delay(2000);
  #endif

  // -------------------------------------------------------------------------
  
  // Emergency WiFi start
  if ((WiFi.isConnected() == false) && (ENC_BTN) == LOW) {
      debugMsgMain("Start open AP");
      wifiManager.openAccessPortal();
  }
  
  // -------------------------------------------------------------------------

  debugMsgMain("Start up WDT...");
  enableWatchdog();

  // -------------------------------------------------------------------------

  debugMsgMain("Load counter configuration...");
  counterManager.setCounterValues(ZIN70, cc->counterValuesZIN70);
  counterManager.setCounterValues(ZIN18, cc->counterValuesZIN18);
  counterManager.setTubeType((tube_type_t)cc->tubeType);
 
  debugMsgMain("Got board count: " + String(cc->tubeBoardCount));

  // -------------------------------------------------------------------------

  debugMsgMain("Setup complete");
  digitalWrite(LED_PIN, LOW);
  digitalWrite(IND1Pin, LOW);
  digitalWrite(IND2Pin, LOW);

  // -------------------------------------------------------------------------
  
  // Set the initial state of the display
  outputManager.setBlanked(false);
}

// ************************************************************
// Called every 10mS or so
// ************************************************************
void performOncePerLoop() {
  
  outputManager.outputDisplay();

  // -------------------------------------------------------------------------------

  #ifdef FEATURE_MENU
  menuManager.menuOncePerLoop();
  #endif

  // -------------------------------------------------------------------------------

  wifiManager.manageDNSInOpenAP();

  digitalWrite(IND1Pin, !counterManager.isCounterRunning());
  digitalWrite(IND2Pin, !(cc->tubeType == ZIN70));
}

// ************************************************************
// Called once per second. Trigger all the things that do
// Not need processing continuously multiple times per second
// ************************************************************
void performOncePerSecondProcessing() {
  lastSecondStartMillis = nowMillis;

  // -------------------------------------------------------------------------------
  
  counterManager.counterCount();
  outputManager.loadNumberIntegerValue(counterManager.getCurrentCounterVal());
//  debugMsgMain("Counter value from manager: " + String(counterManager.getCurrentCounterVal()));
  outputManager.updateOncePerSecond();

  // Turn off tubes at the end of the run
  blanked = counterManager.isCounterExpired() || counterManager.isCounterExpiredConfirmed();

  bool ind1Value = false;
  if (counterManager.isCounterExpired()) {
    ind1Value = (secsDelta % 500) > 250;
  } else {
    ind1Value = counterManager.isCounterRunning();
  }

  // These are inverted because the display is active low
  digitalWrite(IND1Pin, !ind1Value);
  digitalWrite(IND2Pin, !cc->tubeType == ZIN70);

  // -------------------------------------------------------------------------------
  
  // Maintain the LED next to the controller
  if (WiFi.status() == WL_CONNECTED) {
    setLedFlashType(0);
  } else {
    setLedFlashType(1);
  }

  // -------------------------------------------------------------------------------

  // Service the menu
  #ifdef FEATURE_MENU
  menuManager.menuOncePerSecond();
  #endif
  
  // -------------------------------------------------------------------------------

  if (lastExpired != counterManager.isCounterExpired()) {
    if (lastExpired == false && counterManager.isCounterExpired() == true) {
      debugMsgMain("Counter has just expired!");
      playTune();
    }
  }
  lastExpired = counterManager.isCounterExpired();

  // -------------------------------------------------------------------------------
  
  debugManager.debugAutoOffCheck();

  // -------------------------------------------------------------------------------

  feedWatchdog();
}

// ************************************************************
// Called once per minute
// ************************************************************
void performOncePerMinuteProcessing() {
  debugMsgMain("---> OncePerMinuteProcessing");
  // Usage stats
  cs->uptimeMins++;

  if(counterManager.isCounterInhibited() == false) {
    debugMsgMain("Digit burn ---> " + counterManager.getCurrentCounterValString());
  } else
  if (counterManager.isCounterRunning()) {
    debugMsgMain("Counter Running ---> " + counterManager.getCounterValuesCurrent() + " Repetitons: " + String(counterManager.getRepetitionsCurrent()));
  } else {
    if (counterManager.isCounterExpired()) {
      debugMsgMain("Counter Expired!");
      playTune();
    } else {
      debugMsgMain("Counter Stopped!");
    }
  }
}

// ************************************************************
// Called once per hour
// ************************************************************
void performOncePerHourProcessing() {
  debugMsgMain("---> OncePerHourProcessing");
}

// ************************************************************
// Called once per day
// ************************************************************
void performOncePerDayProcessing() {
  debugMsgMain("---> OncePerDayProcessing");

  spiffsStorage.saveStatsToSpiffs();
}

// ************************************************************
// Main loop
// ************************************************************
void loop()
{
  nowMillis = millis();

  if (lastSecondStartMillis > nowMillis) {
    // rollover
    lastSecondStartMillis = 0;
  }

  // -------------------------------------------------------------------------------

  performOncePerLoop();

  if (lastSecond != second()) {
    lastSecond = second();
    performOncePerSecondProcessing();

    if ((second() == 0) && (!triggeredThisSec)) {
      if ((minute() == 0)) {
        if (hour() == 0) {
          performOncePerDayProcessing();
        }
        performOncePerHourProcessing();
      }
      performOncePerMinuteProcessing();
    }

    // Make sure we don't call multiple times
    triggeredThisSec = true;
    if ((second() > 0) && triggeredThisSec) {
      triggeredThisSec = false;
    }
  }

  // Calculate the intra second millis
  secsDeltaAbs = (nowMillis - lastSecondStartMillis);
  if (secsDeltaAbs > 1000) {secsDeltaAbs = 1000;}
  if (secsDeltaAbs < 0) {secsDeltaAbs = 0;}
  upOrDown = (second() % 2) == 0;
  
  if (upOrDown) {
    secsDelta = secsDeltaAbs;
  } else {
    secsDelta = 1000 - secsDeltaAbs;
  }

  // Read and debounce buttons 
  if (btn1ReadMillis > nowMillis) {
    if (digitalRead(BTN1Pin) == LOW) {
      btn1ReadMillis = 0;
//      debugMsgMain("Button 1 pressed");
      if (counterManager.isCounterExpired()) {
        counterManager.confirmCounterExpired();
      } else {
        counterManager.toggleCounterRunning();
      }
    }
  }
  if (btn2ReadMillis > nowMillis) {
    if (digitalRead(BTN2Pin) == LOW) {
      btn2ReadMillis = 0;
//      debugMsgMain("Button 2 pressed");
      counterManager.resetCounter();
    }
  }

  delay(10);
}

