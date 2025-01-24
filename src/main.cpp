#include "Defs.h"
#include "Globals.h"
#include "utilities.h"
#include "WiFi.h"
#include "TimerManager.h"
#include "DebugManager.h"
#include "WiFiManager.h"
#include "OutputManager.h"
#include "CounterManager.h"

void setup() {
  // Show that we booted - useful for remote debugging
  pinMode(LED_PIN, OUTPUT);
  for (int i = 0; i < 10 ; i++) {
    digitalWrite(LED_PIN, (i % 2) == 0);   
    delay(25);   
  }

  // -------------------------------------------------------------------------
  // for reliable startup with GPS connected on older versions of the SDK, you
  // might need to change the uart initialisation. Older versions you have to 
  // change line 200 of esp32-hal-uart.c from
  //
  //      uartFlush(uart);
  //  to
  //      uartFlushTxOnly(uart, false);
  //
  // Which causes the receive buffer NOT to be flushed
  // This has been fixed in 6.5.0 
  Serial.begin(SERIAL_BAUD_RATE);

  #ifdef DEBUG
  // Debug for 10 minutes
  debugManager.setDebugAutoOff(600);
  #endif

  // -------------------------------------------------------------------------

  debugMsgMain("Start up GPIOs");
  pinMode(CLKPin, OUTPUT);
  pinMode(DATA1Pin, OUTPUT);
  pinMode(LATCH1Pin, OUTPUT);
  pinMode(DATA2Pin, OUTPUT);
  pinMode(LATCH2Pin, OUTPUT);
  pinMode(DATA3Pin, OUTPUT);
  pinMode(LATCH3Pin, OUTPUT);

  pinMode(BLANKPin, OUTPUT);
  pinMode(PPSPin, OUTPUT);
  digitalWrite(PPSPin, LOW);
  
  pinMode(BTN1Pin, INPUT_PULLUP);
  pinMode(BTN2Pin, INPUT_PULLUP);
  pinMode(BTN3Pin, INPUT_PULLUP);

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
  }

  // -------------------------------------------------------------------------

  debugMsgMain("Start up Timers");

  // Starts the display and the status LED flashing
  startTimers();

  // -------------------------------------------------------------------------
  // First start digit test
  debugMsgMain("Startup digit test");
  for (int i = 0 ; i <= 20 ; i++) {
    outputManager.loadNumberArraySameValue(i%10);
    outputManager.outputDisplay();
    delay(100);
  }

  // -------------------------------------------------------------------------
  
  // Default pins SDA 21, SCL 22 Frequency 400kHz 
  debugMsgMain("Start up I2C...");
  Wire.begin(SDAint, SCLint, 400000L);

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
  
  // Emergency WiFi start
  if ((WiFi.isConnected() == false) && (ENC_BTN) == LOW) {
      debugMsgMain("Start emergency open AP");
      wifiManager.openAccessPortal();
  }
  
  // -------------------------------------------------------------------------

  debugMsgMain("Start up WDT...");
  enableWatchdog();

  counterManager.setCounterValues("10;60;120;60;20;20;20;20;20;100;10");

  counterManager.startCounter();
}

// ************************************************************
// Called every 10mS or so
// ************************************************************
void performOncePerLoop() {
  
  outputManager.outputDisplay();

  // -------------------------------------------------------------------------------

  wifiManager.manageDNSInOpenAP();
}

// ************************************************************
// Called once per second. Trigger all the things that do
// Not need processing continuously multiple times per second
// ************************************************************
void performOncePerSecondProcessing() {
  lastSecondStartMillis = nowMillis;

  // -------------------------------------------------------------------------------
  
  counterManager.counterCount();
  outputManager.loadNumberArrayIntegerValue(counterManager.getCurrentCounterVal());
  outputManager.updateOncePerSecond();

  // -------------------------------------------------------------------------------
  
  // Maintain the LED next to the controller
  if (WiFi.status() == WL_CONNECTED) {
    setLedFlashType(0);
  } else {
    setLedFlashType(1);
  }

  // -------------------------------------------------------------------------------
  
  debugManager.debugAutoOffCheck();

  // -------------------------------------------------------------------------------

  feedWatchdog();
}

// ************************************************************
// called to process switch changes. An interrupt sets a 
// trigger and the mail loop calls this to process the 
// waiting changes
// ************************************************************
void handleSwitchChange(byte mode, bool state) {
  // Nothing
}

// ************************************************************
// Called once per minute
// ************************************************************
void performOncePerMinuteProcessing() {
  debugMsgMain("---> OncePerMinuteProcessing");
  // Usage stats
  cs->uptimeMins++;
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

  delay(10);
}

