/**
 * @file BurnerMenuConfiguration.h
 * @brief Menu system configuration for the Nixie tube burner device.
 *
 * This header declares the menu structure, callbacks, and external references
 * for the burner's OLED menu interface. It defines:
 * - Menu item pointers for main, burn, wifi, and system menus
 * - Dynamic menu builders that rebuild menus when state changes
 * - Callback functions for all menu actions (burn control, WiFi, system)
 * - Loop functions for periodic menu updates and input handling
 */

#pragma once

#include "ESP32MenuSystem.h"
#include "Globals.h"
#include "TubeTypes.h"

// External references
extern MenuSystem menuSystem;

// Status screen data
extern StatusData burnerStatus;

// Menu item pointers
extern MenuItem* mainMenu;
extern MenuItem* burnMenu;
extern MenuItem* wifiMenu;
extern MenuItem* systemMenu;

// Dynamic menu builders (called when menu state changes)
void buildBurnMenuDynamic();
void buildWifiMenuDynamic();
void buildSystemMenuDynamic();

// Main menu building function
void buildBurnerMenus();

// Callback functions - Main Menu
void menuOff();

// Callback functions - Burn Menu
void startBurn();
void stopBurn();
void pauseBurn();
void resumeBurn();
void resetBurn();
void toggleTubeType();
void saveDigit0();
void saveDigit1();
void saveDigit2();
void saveDigit3();
void saveDigit4();
void saveDigit5();
void saveDigit6();
void saveDigit7();
void saveDigit8();
void saveDigit9();
void saveDigitK();
void saveCycles();
void saveDigitValues();

// Callback functions - WiFi Menu
void disconnectWifi();
void reconnectPrevious();
void connectWPS();
void smartConfig();
void openAccessPoint();
void scanWiFi();
void enterWiFiSSID();
void saveWiFiSSID();
void enterWiFiPassword();
void saveWiFiPassword();

// Callback functions - System Menu
void restartDevice();
void saveConfig();
void resetWiFiInfo();
#ifdef DEBUG
void debugOn10mins();
#endif

// Menu Loop Functions
void menuOncePerLoop();
void menuOncePerSecond();
void updateStatusScreen();
void handleBurnModeInput();

// Custom status screen callbacks
void renderBurnerStatus(Adafruit_SH1106G* display, uint8_t width, uint8_t height);
bool handleStatusInput(ButtonEvent event);
