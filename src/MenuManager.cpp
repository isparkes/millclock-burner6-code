#include "MenuManager.h"

// -------------------------------------------------------------------------------------------------
//                                         menus defintion
// -------------------------------------------------------------------------------------------------

// ************************************************************
// Main Menu
// ************************************************************
void MenuManager_::mainMenu() {
  debugMsgMnm("Main Menu");
  resetMenu();
  byte menuCount = 1;
  menuMode = menu;

  oledMenu.menuTitle = "Main Menu";
  oledMenu.menuItems[menuCount] = "Burn";            oledMenu.menuActions[menuCount++] = gotoBurnMenu;
  oledMenu.menuItems[menuCount] = "Wifi";            oledMenu.menuActions[menuCount++] = gotoWifiMenu;
  oledMenu.menuItems[menuCount] = "System";          oledMenu.menuActions[menuCount++] = gotoOptionsMenu;
  oledMenu.menuItems[menuCount] = "Menu Off";        oledMenu.menuActions[menuCount++] = menuOff;
  oledMenu.noOfmenuItems = --menuCount;
}

// ************************************************************
// Burn Menu
// ************************************************************
void MenuManager_::burnMenu() {
  resetMenu();
  byte menuCount = 1;
  menuMode = menu;

  oledMenu.menuTitle = "Burn Menu";

  if(counterManager.isCounterInhibited()) {
  } else
  if(counterManager.isCounterRunning()) {
    oledMenu.menuItems[menuCount] = "Stop Burn";       oledMenu.menuActions[menuCount++] = stopBurn;
    oledMenu.menuItems[menuCount] = "Reset Burn";      oledMenu.menuActions[menuCount++] = resetBurn;
  } else {
    oledMenu.menuItems[menuCount] = "Start Burn";      oledMenu.menuActions[menuCount++] = startBurn;
    String tubeType = cc->tubeType == ZIN70 ? "ZIN70" : "ZIN18";
    oledMenu.menuItems[menuCount] = "Tube " + tubeType;oledMenu.menuActions[menuCount++] = toggleTubeType;
  }
  oledMenu.menuItems[menuCount] = "Heat Digit";      oledMenu.menuActions[menuCount++] = gotoSingleBurnMenu;
  oledMenu.menuItems[menuCount] = "Back";            oledMenu.menuActions[menuCount++] = backToMain;
  oledMenu.noOfmenuItems = --menuCount;
}

// ************************************************************
// Burn Single Digit Menu
// ************************************************************
void MenuManager_::burnSingleDigitMenu() {
  resetMenu();
  byte menuCount = 1;
  menuMode = menu;

  oledMenu.menuTitle = "Single Burn Menu";
  for (byte i=0; i<10; i++) {
    oledMenu.menuItems[menuCount] = String(i);       oledMenu.menuActions[menuCount++] = setDigit;
  }
  if (cc->tubeType == ZIN70) {
    oledMenu.menuItems[menuCount] = "10";            oledMenu.menuActions[menuCount++] = setDigit;
  }
  oledMenu.menuItems[menuCount] = "Off ";            oledMenu.menuActions[menuCount++] = setDigit;
  oledMenu.menuItems[menuCount] = "Back";            oledMenu.menuActions[menuCount++] = backToBurn;
  oledMenu.noOfmenuItems = --menuCount;
}

// ************************************************************
// Wifi Menu
// ************************************************************
void MenuManager_::wifiMenu() {
  resetMenu();
  menuMode = menu;
  byte menuCount = 1;

  String wcTag = "("+String(wifiManager.getLastScanResultCount())+")";
  if (WiFi.isConnected()) {
    oledMenu.menuTitle = "WiFi Menu";           
    oledMenu.menuItems[menuCount] = "Disconnect WiFi";         oledMenu.menuActions[menuCount++] = disconnectWifi;
    oledMenu.menuItems[menuCount] = "Back";                    oledMenu.menuActions[menuCount++] = backToMain;
  } else {
    oledMenu.menuTitle = "WiFi Menu";
    if (wifiManager.wifiCredentialsReceived()) {
      oledMenu.menuItems[menuCount] = "Reconnect previous";    oledMenu.menuActions[menuCount++] = reconnectPrevious;
    }
    oledMenu.menuItems[menuCount] = "Connect with WPS";        oledMenu.menuActions[menuCount++] = connectWPS;
    oledMenu.menuItems[menuCount] = "Start SmartConfig";       oledMenu.menuActions[menuCount++] = smartConfig;
    oledMenu.menuItems[menuCount] = "Open Access Point";       oledMenu.menuActions[menuCount++] = openAccessPoint;
    oledMenu.menuItems[menuCount] = "Scan Wifi";               oledMenu.menuActions[menuCount++] = scanWiFi;
    oledMenu.menuItems[menuCount] = "Select WiFi " + wcTag;    oledMenu.menuActions[menuCount++] = showWifiSelection;
    oledMenu.menuItems[menuCount] = "Enter SSID";              oledMenu.menuActions[menuCount++] = enterWiFiSSID;
    oledMenu.menuItems[menuCount] = "Enter password";          oledMenu.menuActions[menuCount++] = enterWiFiPassword;
    oledMenu.menuItems[menuCount] = "Back";                    oledMenu.menuActions[menuCount++] = backToMain;
  }
  oledMenu.noOfmenuItems = --menuCount;
}

// ************************************************************
// System Menu
// ************************************************************
void MenuManager_::systemMenu() {
  resetMenu();
  menuMode = menu;
  byte menuCount = 1;

  oledMenu.menuTitle = "System";
  oledMenu.menuItems[menuCount] = "Restart Device"; oledMenu.menuActions[menuCount++] = restartClock;
  oledMenu.menuItems[menuCount] = "Save config";    oledMenu.menuActions[menuCount++] = saveConfig;
  String status = cc->WifiOnAtStart ? "off" : "on";
  oledMenu.menuItems[menuCount] = "WiFi at start: "+ status; oledMenu.menuActions[menuCount++] = toggleWiFiAtStart;
  #ifdef DEBUG
  oledMenu.menuItems[menuCount] = "Debug on 10m";   oledMenu.menuActions[menuCount++] = debugOn10mins;
  #endif
  oledMenu.menuItems[menuCount] = "Reset WiFi";     oledMenu.menuActions[menuCount++] = resetWiFiInfo;
  oledMenu.menuItems[menuCount] = "Back";           oledMenu.menuActions[menuCount++] = backToMain;
  oledMenu.noOfmenuItems = --menuCount;
}

// ************************************************************
// Select WiFi network from those scanned
// ************************************************************
void MenuManager_::wifiSelectMenu() {
  resetMenu();
  menuMode = menu;
  byte menuCount = 1;

  oledMenu.menuTitle = "Select network";

  int numberOfEntries = wifiManager.getLastScanResultCount() < maxmenuItems ? wifiManager.getLastScanResultCount() : maxmenuItems;
  
  // Leave some room for the "Back" option
  numberOfEntries--;
  debugMsgMnm("Showing entries: " + String(numberOfEntries));

  if (wifiManager.getLastScanResultCount() < numberOfEntries) numberOfEntries = wifiManager.getLastScanResultCount();
  for (int i = 0; i < numberOfEntries ; i++) {
    oledMenu.menuItems[menuCount] = wifiManager.getLastScanResultSSID(i); oledMenu.menuActions[menuCount++] = selectWiFiSSID;
  }
  oledMenu.menuItems[menuCount] = "Back"; oledMenu.menuActions[menuCount++] = backToMain;
  oledMenu.noOfmenuItems = --menuCount;
}

// -------------------------------------------------------------------------------------------------
//                                         actiona defintion
// -------------------------------------------------------------------------------------------------

// ************************************************************
// actions for menu selections are put in here
// ************************************************************
void MenuManager_::menuActions(menuTargets selectedAction) {
  switch (selectedAction) {
    // --------------------------------------------------
    // Top Level Menu & Management
    case noTarget: {
      break;
    }
    case unmappedOption: {
      debugMsgMnm("Unmapped option");
      mainMenu();
      break;
    }
    case backToMain: {
      mainMenu();
      break;
    }
    case backToBurn: {
      burnMenu();
      break;
    }
    case gotoBurnMenu: {
      burnMenu();
      break;
    }
    case gotoSingleBurnMenu: {
      burnSingleDigitMenu();
      break;
    }
    case gotoWifiMenu: {
      wifiMenu();
      break;
    }
    case gotoOptionsMenu: {
      systemMenu();
      break;
    }
    case menuOff: {
      resetMenu();
      break;
    }

    // --------------------------------------------------
    // "Main Menu Items"

    // --------------------------------------------------
    // "Burn Menu Items"
    case toggleTubeType: {
      switch (cc->tubeType) {
        case ZIN70: {
          cc->tubeType = ZIN18;
          counterManager.setTubeType(ZIN18);
          break;
        }
        case ZIN18: {
          cc->tubeType = ZIN70;
          counterManager.setTubeType(ZIN70);
          break;
        }
      }
      burnMenu();
      break;
    }
    case stopBurn: {
      counterManager.stopCounter();
      burnMenu();
      break;
    }
    case startBurn: {
      counterManager.startCounter();
      burnMenu();
      break;
    }
    case resetBurn: {
      counterManager.resetCounter();
      burnMenu();
      break;
    }
    case setDigit: {
      debugMsgMnm("Got digit: " + getMenuOptionSelectedText());
      if (getMenuOptionSelectedText().equals("Off ")) {
        counterManager.setDigit(255);  // special code to turn off heating
      } else {
        byte setDigit = getMenuOptionSelectedText().toInt();
        counterManager.setDigit(setDigit);
      }
      burnMenu();
      break;
    }

    // --------------------------------------------------
    // "WiFi Menu Items"
    case reconnectPrevious: {
      wifiManager.connectToLastAP();
      wifiMenu();
      break;
    }
    case connectWPS: {
      wifiManager.connectWithWPS();
      wifiMenu();
      break;
    }
    case smartConfig: {
      wifiManager.startSmartConfig();
      wifiMenu();
      break;
    }
    case openAccessPoint: {
      wifiManager.openAccessPortal();
      wifiMenu();
      break;
    }
    case scanWiFi: {
      wifiManager.startScanWiFiNetworks();
      wifiMenu();
      break;
    }
    case selectWiFiSSID: {
      setWiFiSSIDFromSelection();
      wifiMenu();
      break;
    }
    case showWifiSelection: {
      wifiSelectMenu();
      break;
    }
    case disconnectWifi: {
      wifiManager.disconnectWiFi();
      wifiMenu();
      break;
    }
    case enterWiFiPassword: {
      setStringValue("Enter WiFi password", saveWiFiPassword, cc->WiFiPassword);
      break;
    }
    case saveWiFiPassword: {
      cc->WiFiPassword = getMenuValueEnteredText();
      debugMsgMnm("Set wiFi pw: " + cc->WiFiPassword);
      wifiMenu();
      break;
    }
    case enterWiFiSSID: {
      setStringValue("Enter WiFi name", saveWiFiSSID, cc->WiFiSSID);
      break;
    }
    case saveWiFiSSID: {
      cc->WiFiSSID = getMenuValueEnteredText();
      debugMsgMnm("Set SSID: " + cc->WiFiSSID);
      wifiMenu();
      break;
    }

    // --------------------------------------------------
    // "System Menu Items"
    case restartClock: {
      spiffsStorage.saveStatsToSpiffs();
      flashMenuMessage("Restart","Restarting\nburner\ndevice now");
      delay(1000);
      ESP.restart();
      break;
    }
    case saveConfig: {
      spiffsStorage.saveConfigToSpiffs();
      systemMenu();
      break;
    }
    case toggleWiFiAtStart: {
      cc->WifiOnAtStart = ! cc->WifiOnAtStart;
      systemMenu();
      break;
    }
    #ifdef DEBUG
    case debugOn10mins: {
      debugManager.setDebugAutoOff(600);
      systemMenu();
      break;
    }
    #endif
    case resetWiFiInfo: {
      resetWiFi();
      systemMenu();
      break;
    }
  }

  oledMenu.selectedMenuItem = noTarget;

}  // menuActions

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
//                     menu operations code - not usually changed per application
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// ************************************************************
// Set up a string on the display for entry
// ************************************************************
void MenuManager_::setStringValue(String title, menuTargets target, String initialValue) {
  resetMenu();                           // clear any previous menu
  menuMode = stringValue;                // enable value entry
  
  oledMenu.menuTitle = title;            // title (used to identify which number was entered)
  oledMenu.mValueLow = 0;                // minimum value allowed - this refers to the character set!
  oledMenu.mValueHigh = 82;              // maximum value allowed - this refers to the character set!
  oledMenu.mValueStep = 1;               // step size
  oledMenu.mValueEntered = 0;            // starting value
  oledMenu.nextTarget = target;          // action to call when button pressed
  oledMenu.enteredString = initialValue;
}

// ************************************************************
// Flash a message to the display
// ************************************************************
void MenuManager_::flashMenuMessage(String heading, String message) {
  // Only flash the message if the display is already on
  if (!oled.getBlanked()) {
    resetTimeouts();
    flashTimeout = FLASH_TIME;
    displayMessage(heading, message);
  }
} 

// ************************************************************
// Flash a message to the display
// ************************************************************
void MenuManager_::clearFlashMenuMessage() {
  flashTimeout = 0;
} 

// ************************************************************
// Add a scrolling message to the display
// ************************************************************
void MenuManager_::scrollMenuMessage(String message) {
  // Only show the message if the display is already on
  if (!oled.getBlanked()) {
    resetTimeouts();
    oled.showScrollingMessage(message);
  }
} 

// ************************************************************
// Button debounce (rotary encoder)
// ************************************************************
void MenuManager_::reUpdateButton() {
    bool tReading = digitalRead(ENC_BTN);               // read current button state
    if (tReading != rotaryEncoder.encoderPrevButton) rotaryEncoder.reLastButtonChange = nowMillis;     // if it has changed reset timer
    if ( (unsigned long)(nowMillis - rotaryEncoder.reLastButtonChange) > rotaryEncoder.reDebounceDelay ) {  // if button state is stable
      if (rotaryEncoder.encoderPrevButton == rotaryEncoder.reButtonPressedState) {
        if (rotaryEncoder.reButtonDebounced == 0) {     // if the button has been pressed
          rotaryEncoder.reButtonPressed = 1;            // flag set when the button has been pressed
          serviceStatusDisplayClick();
        }
        rotaryEncoder.reButtonDebounced = 1;            // debounced button status  (1 when pressed)
      } else {
        rotaryEncoder.reButtonDebounced = 0;
      }
    }

    if (rotaryEncoder.reButtonDebounced == 1) {
      resetTimeouts();
    }
    rotaryEncoder.encoderPrevButton = tReading;         // update last state read
}

// ************************************************************
// Service the status display (when no menu is shown)
// ************************************************************
void MenuManager_::serviceStatusDisplayClick() {
  debugMsgMnm("Encoder button pressed");
  if (menuMode == off) mainMenu();        // start the default menu
}


// ************************************************************
// When we are in a menu, do the actions needed to service it
// ************************************************************
void MenuManager_::serviceMenu() {
  if (rotaryEncoder.encoder0Pos >= TICKS_PER_MOVE) {
    rotaryEncoder.encoder0Pos -= TICKS_PER_MOVE;
    oledMenu.highlightedMenuItem++;
    oledMenu.lastMenuActivity = nowMillis;
    oledMenu.needUpdate = true;
  }
  if (rotaryEncoder.encoder0Pos <= -TICKS_PER_MOVE) {
    rotaryEncoder.encoder0Pos += TICKS_PER_MOVE;
    oledMenu.highlightedMenuItem--;
    oledMenu.lastMenuActivity = nowMillis;
    oledMenu.needUpdate = true;
  }
  if (rotaryEncoder.reButtonPressed == 1) {
    oledMenu.selectedMenuItem = oledMenu.highlightedMenuItem;
    oledMenu.lastMenuActivity = nowMillis;
    oledMenu.needUpdate = true;
    debugMsgMnm("menu '" + oledMenu.menuTitle + "' item '" + oledMenu.menuItems[oledMenu.highlightedMenuItem] + "' selected");
  }

  if (oledMenu.needUpdate) {
    const int _centreLine = displayMaxLines / 2 + 1;    // mid list point
    oled.clearDisplay();
    oled.setTextColor(WHITE);

    // verify valid highlighted item
    if (oledMenu.highlightedMenuItem > oledMenu.noOfmenuItems) oledMenu.highlightedMenuItem = oledMenu.noOfmenuItems;
    if (oledMenu.highlightedMenuItem < 1) oledMenu.highlightedMenuItem = 1;

    // title
    oled.setCursor(0, 0);
    if (menuLargeText) {
      oled.setTextSize(2);
      oled.println(oledMenu.menuItems[oledMenu.highlightedMenuItem].substring(0, MaxmenuTitleLength));
    } else {
      if (oledMenu.menuTitle.length() > MaxmenuTitleLength) oled.setTextSize(1);
      else oled.setTextSize(2);
      oled.println(oledMenu.menuTitle);
    }
    oled.drawLine(0, topLine-1, oled.width(), topLine-1, WHITE);       // draw horizontal line under title

    // menu
    oled.setTextSize(1);
    oled.setCursor(0, topLine);
    for (int i=1; i <= displayMaxLines; i++) {
      int item = oledMenu.highlightedMenuItem - _centreLine + i;
      if (item == oledMenu.highlightedMenuItem) oled.setTextColor(BLACK, WHITE);
      else oled.setTextColor(WHITE);
      if (item > 0 && item <= oledMenu.noOfmenuItems) oled.println(oledMenu.menuItems[item]);
      else oled.println(" ");
    }

    oled.outputDisplay();
    oledMenu.needUpdate = false;
  }
}


// ************************************************************
// Service value entry
// ************************************************************
void MenuManager_::serviceValue() {
  // If we timed out, just reset
  if (configTimeout == 0) {
    resetMenu();
  }

  if (rotaryEncoder.encoder0Pos >= TICKS_PER_MOVE) {
    rotaryEncoder.encoder0Pos -= TICKS_PER_MOVE;
    oledMenu.mValueEntered-= oledMenu.mValueStep;
    oledMenu.lastMenuActivity = nowMillis;
    oledMenu.needUpdate = true;
  }
  if (rotaryEncoder.encoder0Pos <= -TICKS_PER_MOVE) {
    rotaryEncoder.encoder0Pos += TICKS_PER_MOVE;
    oledMenu.mValueEntered+= oledMenu.mValueStep;
    oledMenu.lastMenuActivity = nowMillis;
    oledMenu.needUpdate = true;
  }
  if (oledMenu.mValueEntered < oledMenu.mValueLow) {
    oledMenu.mValueEntered = oledMenu.mValueLow;
    oledMenu.lastMenuActivity = nowMillis;
    oledMenu.needUpdate = true;
  }
  if (oledMenu.mValueEntered > oledMenu.mValueHigh) {
    oledMenu.mValueEntered = oledMenu.mValueHigh;
    oledMenu.lastMenuActivity = nowMillis;
    oledMenu.needUpdate = true;
  }

  if (oledMenu.needUpdate) {
    if (menuMode == value) {
      const int _valueSpacingX = 30;      // spacing for the displayed value y position
      const int _valueSpacingY = 5;       // spacing for the displayed value y position
      oled.clearDisplay();
      oled.setTextColor(WHITE);

      // title
      oled.setCursor(0, 0);
      if (oledMenu.menuTitle.length() > MaxmenuTitleLength) oled.setTextSize(1);
      else oled.setTextSize(2);
      oled.println(oledMenu.menuTitle);
      oled.drawLine(0, topLine-1, oled.width(), topLine-1, WHITE);       // draw horizontal line under title

      // value selected
      oled.setCursor(_valueSpacingX, topLine + _valueSpacingY);
      oled.setTextSize(3);
      oled.println(String(oledMenu.mValueEntered));

      // range
      oled.setCursor(0, oled.height() - lineSpace1 - 1 );   // bottom of display
      oled.setTextSize(1);
      oled.println(String(oledMenu.mValueLow) + " to " + String(oledMenu.mValueHigh));

      // bar
      int Tlinelength = map(oledMenu.mValueEntered, oledMenu.mValueLow, oledMenu.mValueHigh, 0 , oled.width());
      oled.drawLine(0, oled.height()-1, Tlinelength, oled.height()-1, WHITE);

      oled.outputDisplay();
      oledMenu.needUpdate = false;
    } else

    if (menuMode == stringValue) {
      const int _valueSpacingY = 5;       // spacing for the displayed value y position
      oled.clearDisplay();
      oled.setTextColor(WHITE);

      // title
      oled.setCursor(0, 0);
      if (oledMenu.menuTitle.length() > MaxmenuTitleLength) oled.setTextSize(1);
      else oled.setTextSize(2);
      oled.println(oledMenu.menuTitle);
      oled.drawLine(0, topLine-1, oled.width(), topLine-1, WHITE);       // draw horizontal line under title

      // value selected
      switch (oledMenu.mValueEntered) {
        case BACKSPACE: {
          oled.setCursor(30, topLine + _valueSpacingY);
          oled.setTextSize(2);
          oled.println("DELETE");
          break;
        }
        case DONE: {
          oled.setCursor(40, topLine + _valueSpacingY);
          oled.setTextSize(2);
          oled.println("DONE");
          break;
        }
        case RESTART: {
          oled.setCursor(26, topLine + _valueSpacingY);
          oled.setTextSize(2);
          oled.println("RESTART");
          break;
        }
        default: {
          oled.setCursor(52, topLine + _valueSpacingY);
          oled.setTextSize(3);
          oled.println(CHARSET.substring(oledMenu.mValueEntered, oledMenu.mValueEntered+1));
        }
      }

      // range
      oled.setCursor(0, oled.height() - lineSpace1 - 1 );   // bottom of display
      oled.setTextSize(1);
      if (oledMenu.enteredString.length() == 0) {
        oled.println("Enter value then DONE");
      } else {
        String displayString = oledMenu.enteredString;
        int displayStrLen = oledMenu.enteredString.length();
        if (displayStrLen > 16) {
          displayString = "..." + oledMenu.enteredString.substring(displayStrLen-14, displayStrLen);
        }
        
        oled.println(String("<" + displayString + ">"));
      }
      oled.outputDisplay();
      oledMenu.needUpdate = false;

      // bar
      int Tlinelength = map(oledMenu.mValueEntered, oledMenu.mValueLow, oledMenu.mValueHigh, 0 , oled.width());
      oled.drawLine(0, oled.height()-1, Tlinelength, oled.height()-1, WHITE);

    }
  }

  reUpdateButton();        // check status of button
}

// ************************************************************
// create a menu from a list
// e.g.       String tList[]={"main menu", "2", "3", "4", "5", "6"};
//            createList("demo_list", 6, &tList[0]);
// ************************************************************
void MenuManager_::createList(String _title, int _noOfElements, String *_list) {
  resetMenu();                                // clear any previous menu
  menuMode = menu;                            // enable menu mode
  oledMenu.noOfmenuItems = _noOfElements;     // set the number of items in this menu
  oledMenu.menuTitle = _title;                // menus title (used to identify it)

  for (int i=1; i <= _noOfElements; i++) {
    oledMenu.menuItems[i] = _list[i-1];       // set the menu items
  }
}

// ************************************************************
// Display a multi line message
// 21 characters per line, use "\n" for next line
// assistant:  <     line 1        ><     line 2        ><     line 3        ><     line 4         >
// ************************************************************
void MenuManager_::displayMessage(String _title, String _message) {
  resetMenu();
  menuMode = message;

  oled.clearDisplay();
  oled.setTextColor(WHITE);

  // title
  oled.setCursor(0, 0);
  if (menuLargeText) {
    oled.setTextSize(2);
    oled.println(_title.substring(0, MaxmenuTitleLength));
  } else {
    if (_title.length() > MaxmenuTitleLength) oled.setTextSize(1);
    else oled.setTextSize(2);
    oled.println(_title);
  }

  // message
  oled.setCursor(0, topLine);
  oled.setTextSize(1);
  oled.println(_message);

  oled.outputDisplay();
 }

// ************************************************************
// Reset the menu system
// reset all menu variables / flags
// ************************************************************
void MenuManager_::resetMenu() {
  menuMode = off;
  oledMenu.selectedMenuItem = noTarget;
  rotaryEncoder.encoder0Pos = 0;
  oledMenu.noOfmenuItems = 0;
  oledMenu.menuTitle = "";
  oledMenu.highlightedMenuItem = 0;
  oledMenu.mValueEntered = 0;
  rotaryEncoder.reButtonPressed = 0;

  oledMenu.lastMenuActivity = nowMillis;

  oled.blankDisplay();
}


// ************************************************************
//                     -interrupt for rotary encoder
// rotary encoder interrupt routine to update position counter when turned
// interrupt info: https://www.gammon.com.au/forum/bbshowpost.php?id=11488
// ************************************************************
void ICACHE_RAM_ATTR MenuManager_::doEncoder() {
  bool pinA = digitalRead(ENC_APin);
  bool pinB = digitalRead(ENC_BPin);
  int delta = 0;

  encoderToggle = !encoderToggle;

  if ( (rotaryEncoder.encoderPrevA == pinA && rotaryEncoder.encoderPrevB == pinB) ) return;  // no change since last time (i.e. reject bounce)

  // same direction (alternating between 0,1 and 1,0 in one direction or 1,1 and 0,0 in the other direction)
       if (rotaryEncoder.encoderPrevA == 1 && rotaryEncoder.encoderPrevB == 0 && pinA == 0 && pinB == 1) {rotaryEncoder.encoder0Pos -= 1; delta = -1;}
  else if (rotaryEncoder.encoderPrevA == 0 && rotaryEncoder.encoderPrevB == 1 && pinA == 1 && pinB == 0) {rotaryEncoder.encoder0Pos -= 1; delta = -1;}
  else if (rotaryEncoder.encoderPrevA == 0 && rotaryEncoder.encoderPrevB == 0 && pinA == 1 && pinB == 1) {rotaryEncoder.encoder0Pos += 1; delta = 1;}
  else if (rotaryEncoder.encoderPrevA == 1 && rotaryEncoder.encoderPrevB == 1 && pinA == 0 && pinB == 0) {rotaryEncoder.encoder0Pos += 1; delta = 1;}

  // change of direction
  else if (rotaryEncoder.encoderPrevA == 1 && rotaryEncoder.encoderPrevB == 0 && pinA == 0 && pinB == 0) {rotaryEncoder.encoder0Pos += 1; delta = 1;}
  else if (rotaryEncoder.encoderPrevA == 0 && rotaryEncoder.encoderPrevB == 1 && pinA == 1 && pinB == 1) {rotaryEncoder.encoder0Pos += 1; delta = 1;}
  else if (rotaryEncoder.encoderPrevA == 0 && rotaryEncoder.encoderPrevB == 0 && pinA == 1 && pinB == 0) {rotaryEncoder.encoder0Pos -= 1; delta = -1;}
  else if (rotaryEncoder.encoderPrevA == 1 && rotaryEncoder.encoderPrevB == 1 && pinA == 0 && pinB == 1) {rotaryEncoder.encoder0Pos -= 1; delta = -1;}

  // update previous readings
  rotaryEncoder.encoderPrevA = pinA;
  rotaryEncoder.encoderPrevB = pinB;

  // Reset the display timeouts if we have movement
  resetTimeouts();
}

// ************************************************************
// Reset the display timeout
// ************************************************************
void MenuManager_::resetTimeouts() {
  // first press: wake up
  if (oledTimeout == 0) {
    debugMsgMnm("OLED: ON");
    resetDisplay = true;
  } else if (menuMode > off) {
    configTimeout = CONFIG_TIME;
    oledMenu.needUpdate = true;
  }
  oledTimeout = OLED_ON_TIME;
}

// ************************************************************
// Count down the various timeouts
// ************************************************************
void MenuManager_::countdownMenuTimeouts() {
  if (flashTimeout > 0) {
    flashTimeout--;
    if(flashTimeout == 0) {
      oled.clearDisplay();
    }
  }

  if (configTimeout > 0) {
    configTimeout--;
    if (configTimeout == 0) {
      oled.clearDisplay();
    }
  }

  if (oledTimeout > 0) {
    oledTimeout--;
    if (oledTimeout == 0) {
      oled.blankDisplay();
      debugMsgMnm("OLED: OFF");
      menuMode = off;
    }
  }

  if ((oledTimeout == -1) && (OLED_ON_TIME > 0)) {
    oledTimeout = OLED_ON_TIME;
  }
}

// ************************************************************
// Get the current position of the rotary encoder
// ************************************************************
int MenuManager_::getCurrentEncoderPos() {
  return rotaryEncoder.encoder0Pos;
}

// ************************************************************
// Set the WiFi SSID from the selected menu item
// ************************************************************
void MenuManager_::setWiFiSSIDFromSelection() {
  debugMsgMnm("Selected option = " + String(oledMenu.selectedMenuItem));
  String selectedWiFi = wifiManager.getLastScanResultSSID(oledMenu.selectedMenuItem - 1);
  debugMsgMnm("Selected WiFi = " + selectedWiFi);
  cc->WiFiSSID = selectedWiFi;
}

// ************************************************************
// After a menu item is selected, return the value entered
// ************************************************************
byte MenuManager_::getMenuOptionSelected() {
  return oledMenu.selectedMenuItem;
}

// ************************************************************
// After a menu item is selected, return the text selected
// ************************************************************
String MenuManager_::getMenuOptionSelectedText() {
  return oledMenu.menuItems[oledMenu.highlightedMenuItem];
}

// ************************************************************
// After a menu item is selected, return the text selected
// ************************************************************
String MenuManager_::getMenuValueEnteredText() {
  return oledMenu.enteredString;
}

// ************************************************************
// Main entry point for the menu mangement:
//  - Build and update the status display
//  - Service the menu system and value entry system
// ************************************************************
void MenuManager_::menuOncePerLoop() {
  reUpdateButton();               // update rotary encoder button status (if pressed activate default menu)

  if (menuMode == off) return;    // if menu system is turned off do nothing more

  debugMsgMnmX("Menu mode: " + String(menuMode) + " sel: " + String(oledMenu.selectedMenuItem) + " high: " + String(oledMenu.highlightedMenuItem) + " val: " + String(oledMenu.mValueEntered) + " conf: " + String(configTimeout) + " flash: " + String(flashTimeout) + " oled: " + String(oledTimeout));

  if (resetDisplay) {
    // Re-initialise
    oled.setUp();
    resetDisplay = false;
  }

  if ( configTimeout == 0 ) {
    resetMenu();
    return;
  }

  switch (menuMode) {
    case menu:
      serviceMenu();
      menuActions(oledMenu.menuActions[oledMenu.selectedMenuItem]);
      break;

    case value:
      serviceValue();
      if (rotaryEncoder.reButtonPressed) {
        debugMsgMnm("Button pressed: value: "+ String(oledMenu.mValueEntered));
        rotaryEncoder.reButtonPressed = 0;
        menuActions(oledMenu.nextTarget);
      }
      break;

    case stringValue:
      serviceValue();
      if (rotaryEncoder.reButtonPressed) {
        debugMsgMnm("Button pressed: value: "+ String(oledMenu.mValueEntered));
        rotaryEncoder.reButtonPressed = 0;
        if (oledMenu.mValueEntered == BACKSPACE) {
          oledMenu.enteredString = oledMenu.enteredString.substring(0, oledMenu.enteredString.length() - 1);
        } else if (oledMenu.mValueEntered == RESTART) {
          oledMenu.enteredString = "";
        } else if (oledMenu.mValueEntered == DONE) {
          menuActions(oledMenu.nextTarget);
        } else {
          oledMenu.enteredString = oledMenu.enteredString + CHARSET.substring(oledMenu.mValueEntered, oledMenu.mValueEntered+1);
        }
      }
      break;

    case message:
      if (rotaryEncoder.reButtonPressed == 1) clearFlashMenuMessage();
      break;

    default:
      break;
  }
}

// ************************************************************
// Build the status display
// ************************************************************
void MenuManager_::menuOncePerSecond() {
  // Manage timeouts
  countdownMenuTimeouts();

  // only show status line when not in config/flashmessage mode and display is on
  if (oledTimeout != 0 && configTimeout == 0 && flashTimeout == 0) {
    oled.setTextSize(1);
    oled.showStatusLine();

    oled.clearScrollingMessage();
    oled.setCursor(0, 0);
    if (WiFi.isConnected()) {
      oled.showScrollingMessage("IP: " + WiFi.localIP().toString());
      oled.showScrollingMessage(String(WiFi.getHostname()) + ".local");
      oled.showScrollingMessage(String(WiFi.SSID()));
    } else {
      oled.showScrollingMessage("WiFi not connected");
    }
  }
}

// ************************************************************
// Things that need updating once per hour
// ************************************************************
void MenuManager_::menuOncePerHour() {
  // nothing at present
}

// ************************************************************
// Get the current blanking status of the OLED
// ************************************************************
bool MenuManager_::getOledIsBlanked() {
  return oledTimeout == 0;
}

// ************************************************************
// Capture the confirm button press
// ************************************************************
void MenuManager_::confirmButtonPress() {
}

// ************************************************************
// Capture the back button press
// ************************************************************
void MenuManager_::backButtonPress() {
}

// ************************************************************
//                        -internal plumbing
// ************************************************************
void IRAM_ATTR doEncoderWrapper() {
  portENTER_CRITICAL_ISR(&encoderMux);
  menuManager.doEncoder();
  portEXIT_CRITICAL_ISR(&encoderMux);
}

// ************************************************************
// Operation setup for the menu system
// ************************************************************
void MenuManager_::setupMenuManager() {
  pinMode(ENC_BTN, INPUT_PULLUP);
  pinMode(ENC_APin, INPUT);
  pinMode(ENC_BPin, INPUT);

  // Interrupt for reading the rotary encoder position
  rotaryEncoder.encoder0Pos = 0;
  attachInterrupt(digitalPinToInterrupt(ENC_APin), doEncoderWrapper, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_BPin), doEncoderWrapper, CHANGE);
}

// ************************************************************
// Library internal singleton wiring
// ************************************************************
MenuManager_ &MenuManager_::getInstance() {
  static MenuManager_ instance;
  return instance;
}

MenuManager_ &menuManager = menuManager.getInstance();