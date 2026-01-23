/*
 * BurnerMenuConfiguration.cpp
 *
 * Configuration of the menu system for the Burner application.
 */

#include "BurnerMenuConfiguration.h"
#include "CounterManager.h"
#include "WiFiManager.h"
#include "SpiffsStorage.h"
#include "DebugManager.h"
#include <Arduino.h>

// External references
extern MenuSystem menuSystem;

// Menu item pointers
MenuItem* mainMenu;
MenuItem* burnMenu;
MenuItem* wifiMenu;
MenuItem* systemMenu;

// Temporary variables for dynamic menu items
int digit0Time, digit1Time, digit2Time, digit3Time, digit4Time;
int digit5Time, digit6Time, digit7Time, digit8Time, digit9Time, digitKTime;
int cyclesValue;

// Char buffers for WiFi string editing (synced with cc->WiFiSSID and cc->WiFiPassword)
char wifiSSIDBuffer[32];
char wifiPasswordBuffer[64];

// Status screen data
StatusData burnerStatus = {
  false,              // wifiConnected
  "0.0.0.0"          // ipAddress
};

// Burner-specific status labels (not part of generic StatusData)
static const char* statusLabel = "Stopped";

// Static variables for menu loop functions
static unsigned long _lastStatusUpdate = 0;

// ============================================================================
// Callback functions - Main Menu
// ============================================================================

void menuOff() {
  debugMsgMnm("[ACTION] Menu Off - returning to status");
  menuSystem.showStatusScreen();
}

// ============================================================================
// Callback functions - Burn Menu
// ============================================================================

void startBurn() {
  debugMsgMnm("[ACTION] Start Burn");
  counterManager.startCounter();
  // Rebuild menu to show pause/stop options
  buildBurnMenuDynamic();
}

void stopBurn() {
  debugMsgMnm("[ACTION] Stop Burn");
  counterManager.stopCounter();
  buildBurnMenuDynamic();
}

void pauseBurn() {
  debugMsgMnm("[ACTION] Pause Burn");
  counterManager.pauseCounter();
  buildBurnMenuDynamic();
}

void resumeBurn() {
  debugMsgMnm("[ACTION] Resume Burn");
  counterManager.startCounter();
  buildBurnMenuDynamic();
}

void resetBurn() {
  debugMsgMnm("[ACTION] Reset Burn");
  counterManager.resetCounter();
  buildBurnMenuDynamic();
}

void toggleTubeType() {
  debugMsgMnm("[ACTION] Toggle Tube Type");
  counterManager.toggleTubeType();
  cc->tubeType = counterManager.getTubeType();
  buildBurnMenuDynamic();
}

void saveDigit0() { counterManager.setDigitTime(0, digit0Time); buildBurnMenuDynamic(); }
void saveDigit1() { counterManager.setDigitTime(1, digit1Time); buildBurnMenuDynamic(); }
void saveDigit2() { counterManager.setDigitTime(2, digit2Time); buildBurnMenuDynamic(); }
void saveDigit3() { counterManager.setDigitTime(3, digit3Time); buildBurnMenuDynamic(); }
void saveDigit4() { counterManager.setDigitTime(4, digit4Time); buildBurnMenuDynamic(); }
void saveDigit5() { counterManager.setDigitTime(5, digit5Time); buildBurnMenuDynamic(); }
void saveDigit6() { counterManager.setDigitTime(6, digit6Time); buildBurnMenuDynamic(); }
void saveDigit7() { counterManager.setDigitTime(7, digit7Time); buildBurnMenuDynamic(); }
void saveDigit8() { counterManager.setDigitTime(8, digit8Time); buildBurnMenuDynamic(); }
void saveDigit9() { counterManager.setDigitTime(9, digit9Time); buildBurnMenuDynamic(); }
void saveDigitK() { counterManager.setDigitTime(10, digitKTime); buildBurnMenuDynamic(); }

void saveCycles() {
  counterManager.setRepetitions(cyclesValue);
  buildBurnMenuDynamic();
}

void saveDigitValues() {
  debugMsgMnm("[ACTION] Save digit values to SPIFFS");
  cc->counterValuesZIN70 = counterManager.getCounterValues(ZIN70);
  cc->counterValuesZIN18 = counterManager.getCounterValues(ZIN18);
  spiffsStorage.saveConfigToSpiffs();
  buildBurnMenuDynamic();
}

// ============================================================================
// Callback functions - WiFi Menu
// ============================================================================

void disconnectWifi() {
  debugMsgMnm("[ACTION] Disconnect WiFi");
  wifiManager.disconnectWiFi();
  buildWifiMenuDynamic();
}

void reconnectPrevious() {
  debugMsgMnm("[ACTION] Reconnect to previous WiFi");
  wifiManager.connectToLastAP();
}

void connectWPS() {
  debugMsgMnm("[ACTION] Connect with WPS");
  wifiManager.connectWithWPS();
}

void smartConfig() {
  debugMsgMnm("[ACTION] Start SmartConfig");
  wifiManager.startSmartConfig();
}

void openAccessPoint() {
  debugMsgMnm("[ACTION] Open Access Point");
  wifiManager.openAccessPortal();
}

void scanWiFi() {
  debugMsgMnm("[ACTION] Scan WiFi networks");
  wifiManager.startScanWiFiNetworks();
}

void enterWiFiSSID() {
  debugMsgMnm("[ACTION] Enter WiFi SSID");
  // Copy current SSID to buffer for editing
  strncpy(wifiSSIDBuffer, cc->WiFiSSID.c_str(), 31);
  wifiSSIDBuffer[31] = '\0';
}

void saveWiFiSSID() {
  // Copy buffer back to config String
  cc->WiFiSSID = String(wifiSSIDBuffer);
  debugMsgMnm("[ACTION] Save WiFi SSID: " + cc->WiFiSSID);
  buildWifiMenuDynamic();
}

void enterWiFiPassword() {
  debugMsgMnm("[ACTION] Enter WiFi Password");
  // Copy current password to buffer for editing
  strncpy(wifiPasswordBuffer, cc->WiFiPassword.c_str(), 63);
  wifiPasswordBuffer[63] = '\0';
}

void saveWiFiPassword() {
  // Copy buffer back to config String
  cc->WiFiPassword = String(wifiPasswordBuffer);
  debugMsgMnm("[ACTION] Save WiFi Password");
  buildWifiMenuDynamic();
}

// ============================================================================
// Callback functions - System Menu
// ============================================================================

void restartDevice() {
  debugMsgMnm("[ACTION] Restart Device");
  spiffsStorage.saveStatsToSpiffs();
  delay(1000);
  ESP.restart();
}

void saveConfig() {
  debugMsgMnm("[ACTION] Save Configuration");
  spiffsStorage.saveConfigToSpiffs();
}

void resetWiFiInfo() {
  debugMsgMnm("[ACTION] Reset WiFi Information");
  wifiManager.resetWiFiCredentials();
}

#ifdef DEBUG
void debugOn10mins() {
  debugMsgMnm("[ACTION] Debug on for 10 minutes");
  debugManager.setDebugAutoOff(600);
}
#endif

// ============================================================================
// Dynamic Menu Builders
// ============================================================================

void buildBurnMenuDynamic() {
  // Clear existing burn menu
  if (burnMenu != NULL) {
    // Note: ESP32MenuSystem doesn't have a clearMenu function yet
    // We'll recreate the menu
  }

  burnMenu = menuSystem.createMenu("Burn Menu");

  if (counterManager.isCounterRunning()) {
    menuSystem.addAction(burnMenu, "Stop Burn", stopBurn);
    menuSystem.addAction(burnMenu, "Reset Burn", resetBurn);

    if (counterManager.isCounterPaused()) {
      menuSystem.addAction(burnMenu, "Resume Burn", resumeBurn);
    } else {
      menuSystem.addAction(burnMenu, "Pause Burn", pauseBurn);
    }
  } else {
    menuSystem.addAction(burnMenu, "Start Burn", startBurn);

    // Show tube type - use static label
    if (cc->tubeType == ZIN70) {
      menuSystem.addAction(burnMenu, "Tube: ZIN70", toggleTubeType);
    } else {
      menuSystem.addAction(burnMenu, "Tube: ZIN18", toggleTubeType);
    }
  }

  // Add digit time settings
  digit0Time = counterManager.getDigitTime(0);
  menuSystem.addNumericValue(burnMenu, "Digit 0", &digit0Time, 0, 999, 1, "s");

  digit1Time = counterManager.getDigitTime(1);
  menuSystem.addNumericValue(burnMenu, "Digit 1", &digit1Time, 0, 999, 1, "s");

  digit2Time = counterManager.getDigitTime(2);
  menuSystem.addNumericValue(burnMenu, "Digit 2", &digit2Time, 0, 999, 1, "s");

  digit3Time = counterManager.getDigitTime(3);
  menuSystem.addNumericValue(burnMenu, "Digit 3", &digit3Time, 0, 999, 1, "s");

  digit4Time = counterManager.getDigitTime(4);
  menuSystem.addNumericValue(burnMenu, "Digit 4", &digit4Time, 0, 999, 1, "s");

  digit5Time = counterManager.getDigitTime(5);
  menuSystem.addNumericValue(burnMenu, "Digit 5", &digit5Time, 0, 999, 1, "s");

  digit6Time = counterManager.getDigitTime(6);
  menuSystem.addNumericValue(burnMenu, "Digit 6", &digit6Time, 0, 999, 1, "s");

  digit7Time = counterManager.getDigitTime(7);
  menuSystem.addNumericValue(burnMenu, "Digit 7", &digit7Time, 0, 999, 1, "s");

  digit8Time = counterManager.getDigitTime(8);
  menuSystem.addNumericValue(burnMenu, "Digit 8", &digit8Time, 0, 999, 1, "s");

  digit9Time = counterManager.getDigitTime(9);
  menuSystem.addNumericValue(burnMenu, "Digit 9", &digit9Time, 0, 999, 1, "s");

  if (cc->tubeType == ZIN70) {
    digitKTime = counterManager.getDigitTime(10);
    menuSystem.addNumericValue(burnMenu, "Digit K", &digitKTime, 0, 999, 1, "s");
  }

  cyclesValue = counterManager.getRepetitions();
  menuSystem.addNumericValue(burnMenu, "Cycles", &cyclesValue, 1, 99, 1, "");

  menuSystem.addAction(burnMenu, "Save All", saveDigitValues);
}

void buildWifiMenuDynamic() {
  // Clear and rebuild WiFi menu based on connection status
  if (wifiMenu != NULL) {
    // Recreate the menu
  }

  wifiMenu = menuSystem.createMenu("WiFi");

  if (WiFi.isConnected()) {
    menuSystem.addAction(wifiMenu, "Disconnect", disconnectWifi);
  } else {
    if (wifiManager.wifiCredentialsReceived()) {
      menuSystem.addAction(wifiMenu, "Reconnect Prev", reconnectPrevious);
    }
    menuSystem.addAction(wifiMenu, "WPS Connect", connectWPS);
    menuSystem.addAction(wifiMenu, "SmartConfig", smartConfig);
    menuSystem.addAction(wifiMenu, "Access Point", openAccessPoint);
    menuSystem.addAction(wifiMenu, "Scan Networks", scanWiFi);

    // Initialize buffers with current values
    strncpy(wifiSSIDBuffer, cc->WiFiSSID.c_str(), 31);
    wifiSSIDBuffer[31] = '\0';
    strncpy(wifiPasswordBuffer, cc->WiFiPassword.c_str(), 63);
    wifiPasswordBuffer[63] = '\0';

    menuSystem.addStringValue(wifiMenu, "SSID", wifiSSIDBuffer, 32);
    menuSystem.addStringValue(wifiMenu, "Password", wifiPasswordBuffer, 64);
  }
}

void buildSystemMenuDynamic() {
  if (systemMenu != NULL) {
    // Recreate the menu
  }

  systemMenu = menuSystem.createMenu("System");
  menuSystem.addAction(systemMenu, "Restart", restartDevice);
  menuSystem.addAction(systemMenu, "Save Config", saveConfig);

  // Use either-or for WiFi at start instead of action with dynamic label
  menuSystem.addEitherOr(systemMenu, "WiFi@Start", &cc->WifiOnAtStart, "ON", "OFF");

  #ifdef DEBUG
  menuSystem.addAction(systemMenu, "Debug 10m", debugOn10mins);
  #endif

  menuSystem.addAction(systemMenu, "Reset WiFi", resetWiFiInfo);
}

// ============================================================================
// Main Menu Builder
// ============================================================================

void buildBurnerMenus() {
  // Build dynamic submenus first
  buildBurnMenuDynamic();
  buildWifiMenuDynamic();
  buildSystemMenuDynamic();

  // Create main menu and add submenus
  mainMenu = menuSystem.createMenu("MAIN MENU");
  menuSystem.addSubmenu(mainMenu, "Burn", burnMenu);
  menuSystem.addSubmenu(mainMenu, "WiFi", wifiMenu);
  menuSystem.addSubmenu(mainMenu, "System", systemMenu);
  menuSystem.addAction(mainMenu, "Menu Off", menuOff);
  menuSystem.addInfo(mainMenu, "v1.0");

  // Register custom status screen callbacks
  menuSystem.setStatusRenderCallback(renderBurnerStatus);
  menuSystem.setStatusInputCallback(handleStatusInput);
}

// ============================================================================
// Menu Loop Functions
// ============================================================================

void updateStatusScreen() {
  // Update WiFi status
  burnerStatus.wifiConnected = WiFi.isConnected();
  if (burnerStatus.wifiConnected) {
    strncpy(burnerStatus.ipAddress, WiFi.localIP().toString().c_str(), 15);
    burnerStatus.ipAddress[15] = '\0';
  } else {
    strcpy(burnerStatus.ipAddress, "Not connected");
  }

  // Update run state label
  if (counterManager.isCounterExpired()) {
    statusLabel = "EXPIRED";
  } else if (counterManager.isCounterRunning()) {
    if (counterManager.isCounterOverride()) {
      statusLabel = "Override";
    } else if (counterManager.isCounterPaused()) {
      statusLabel = "Paused";
    } else {
      statusLabel = "Running";
    }
  } else {
    statusLabel = "Stopped";
  }
}

// ============================================================================
// Custom Status Screen Rendering (like old OLED.cpp showStatusLine)
// ============================================================================

void renderBurnerStatus(Adafruit_SH1106G* display, uint8_t width, uint8_t height) {
  int16_t x1, y1;
  uint16_t w, h;

  // Main status label - prominently displayed at top
  display->setTextSize(2);
  display->getTextBounds(statusLabel, 0, 0, &x1, &y1, &w, &h);
  display->setCursor((width - w) / 2, 0);
  display->print(statusLabel);

  // Seconds remaining (if running/paused) - below status label
  int secondsRemaining = counterManager.getSecondsRemainingCurrentValue();
  if (secondsRemaining > 0 && !counterManager.isCounterOverride()) {
    display->setTextSize(2);
    char valStr[8];
    snprintf(valStr, 8, "%ds", secondsRemaining);
    display->getTextBounds(valStr, 0, 0, &x1, &y1, &w, &h);
    display->setCursor((width - w) / 2, 18);
    display->print(valStr);
  }

  // Status bar at bottom (like old OLED) - two rows
  const uint8_t boxHeight = 22;
  const uint8_t boxY = height - boxHeight;
  display->drawRect(0, boxY, width, boxHeight, SH110X_WHITE);

  display->setTextSize(1);

  // Row 1: WiFi, Run state, Tube type, Current digit
  display->setCursor(2, boxY + 2);
  display->print(WiFi.isConnected() ? "W" : "w");
  display->print(" ");

  // Run indicator: O/P/R/X/-
  if (counterManager.isCounterExpired()) {
    display->print("X");
  } else if (counterManager.isCounterRunning()) {
    if (counterManager.isCounterOverride()) {
      display->print("O");
    } else if (counterManager.isCounterPaused()) {
      display->print("P");
    } else {
      display->print("R");
    }
  } else {
    display->print("-");
  }
  display->print(" T:");
  display->print((cc->tubeType == ZIN70) ? 70 : 18);
  display->print(" D:");
  display->print(counterManager.getCurrentCounterValString());

  // Row 2: Repetitions
  display->setCursor(2, boxY + 12);
  display->print("R:");
  display->print(counterManager.getRepetitionsCurrent());
  display->print("/");
  display->print(counterManager.getRepetitions());
}

// Handle button input on status screen
// Returns true if event was consumed, false to allow default handling
bool handleStatusInput(ButtonEvent event) {
  switch (event) {
    case BTN_CONFIRM_CLICK:
      // Confirm button: Start/Resume burn if stopped/paused
      if (!counterManager.isCounterRunning()) {
        startBurn();
        return true;
      } else if (counterManager.isCounterPaused()) {
        resumeBurn();
        return true;
      }
      break;

    case BTN_BACK_CLICK:
      // Back button: Pause if running, or stop if paused
      if (counterManager.isCounterRunning() && !counterManager.isCounterPaused()) {
        pauseBurn();
        return true;
      } else if (counterManager.isCounterPaused()) {
        stopBurn();
        return true;
      }
      break;

    case BTN_ENCODER_CLICK:
      // Encoder click: Let default handling enter menu
      return false;

    default:
      break;
  }
  return false;
}

void handleBurnModeInput() {
  // Note: Special burn mode input handling could be added here
  // For example, when burning is active on the status screen:
  // - Encoder turn: pause and select digit override
  // - Encoder press: toggle pause/resume
  //
  // For now, let the ESP32MenuSystem handle all input
  // Users can press encoder to enter menu, then use burn menu
}

void menuOncePerLoop() {
  // Update the menu system (handles encoder, buttons, display)
  menuSystem.update();

  // Handle special burn mode logic
  handleBurnModeInput();

  // Periodically update status (every 100ms)
  if (millis() - _lastStatusUpdate > 100) {
    _lastStatusUpdate = millis();
    updateStatusScreen();
  }
}

void menuOncePerSecond() {
  // Update status screen data
  updateStatusScreen();

  // Check if burn just completed
  if (counterManager.isCounterExpired() && !counterManager.isCounterExpiredConfirmed()) {
    debugMsgMnm("Burn completed!");
    counterManager.confirmCounterExpired();

    // Rebuild burn menu to update state
    buildBurnMenuDynamic();
  }
}
