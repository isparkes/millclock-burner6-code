/*
 * ESP32MenuSystem.h
 * 
 * A comprehensive menu system library for ESP32 with rotary encoder and OLED display
 * Supports hierarchical menus, multiple display modes, and interrupt-driven input
 * 
 * Author: Ian
 * Display: SH1106 OLED 128x64 I2C
 * Input: Rotary Encoder + 2 Buttons (Confirm, Back)
 */

 #ifndef ESP32_MENU_SYSTEM_H
 #define ESP32_MENU_SYSTEM_H
 
 #include <Arduino.h>
 #include <Wire.h>
 #include <Adafruit_GFX.h>
 #include <Adafruit_SH110X.h>
 #include "DebugManager.h"
 
 // Menu item types
 enum MenuItemType {
   MENU_ITEM_SUBMENU,
   MENU_ITEM_ACTION,
   MENU_ITEM_NUMERIC_VALUE,
   MENU_ITEM_STRING_VALUE,
   MENU_ITEM_EITHER_OR,
   MENU_ITEM_INFO
 };
 
 // Display modes
 enum DisplayMode {
   MODE_STATUS_SCREEN,
   MODE_MENU_NAVIGATION,
   MODE_NUMERIC_VALUE,
   MODE_STRING_COMPOSITION,
   MODE_EITHER_OR
 };
 
 // Button event types
 enum ButtonEvent {
   BTN_NONE,
   BTN_CONFIRM_CLICK,
   BTN_BACK_CLICK,
   BTN_ENCODER_CLICK,
   BTN_CONFIRM_LONG,
   BTN_BACK_LONG
 };
 
 // Forward declarations
 class MenuItem;
 class MenuSystem;
 
 // Callback function types
 typedef void (*ActionCallback)(void);
 typedef void (*ValueCallback)(int value);
 typedef void (*StringCallback)(const char* value);
 typedef void (*StatusRenderCallback)(Adafruit_SH1106G* display, uint8_t width, uint8_t height);
 typedef bool (*StatusInputCallback)(ButtonEvent event);  // Return true to consume event
 
 // Status screen data structure
 struct StatusData {
   bool wifiConnected;
   char ipAddress[16];
 };
 
 // Menu Item class
 class MenuItem {
 public:
   char label[21];
   MenuItemType type;
   MenuItem* parent;
   MenuItem* child;  // For submenus
   MenuItem* next;   // Linked list
   
   // Type-specific data
   union {
     ActionCallback actionCallback;
     struct {
       int* valuePtr;
       int minValue;
       int maxValue;
       int step;
       const char* unit;
     } numericData;
     struct {
       char* buffer;
       uint8_t maxLength;
       const char* charset;
     } stringData;
     struct {
       bool* valuePtr;
       const char* option1;
       const char* option2;
     } eitherOrData;
   } data;
   
   MenuItem(const char* lbl, MenuItemType t) : type(t), parent(NULL), child(NULL), next(NULL) {
     strncpy(label, lbl, 20);
     label[20] = '\0';
   }
 };
 
 // Menu System class
 class MenuSystem {
 private:
   // Display
   Adafruit_SH1106G* display;
   uint8_t screenWidth;
   uint8_t screenHeight;
   
   // Input pins
   uint8_t encoderClkPin;
   uint8_t encoderDtPin;
   uint8_t encoderSwPin;
   uint8_t confirmPin;
   uint8_t backPin;
   
   // Menu state
   MenuItem* rootMenu;
   MenuItem* currentMenu;
   MenuItem* selectedItem;
   uint8_t selectedIndex;
   uint8_t scrollOffset;
   uint8_t visibleItems;
   DisplayMode currentMode;
   
   // Numeric value editing state
   int editingValue;
   int editingMin;
   int editingMax;
   int editingStep;
   
   // String editing state
   char* editingBuffer;
   uint8_t editingMaxLen;
   uint8_t editingCursorPos;
   uint8_t editingCharIndex;
   const char* editingCharset;
   
   // Either-Or state
   bool editingEitherOr;
   
   // Encoder state (volatile for ISR)
   volatile int encoderPosition;
   volatile unsigned long lastEncoderTime;
   volatile uint8_t lastEncoderState;
   volatile int encoderPulseCount;  // Count pulses for sensitivity adjustment
   
   // Button state (volatile for ISR)
   volatile bool confirmPressed;
   volatile bool backPressed;
   volatile bool encoderSwPressed;
   volatile unsigned long confirmPressTime;
   volatile unsigned long backPressTime;
   volatile unsigned long encoderSwPressTime;
   
   // Configuration
   const unsigned long debounceDelay = 50;
   const unsigned long longPressDelay = 500;
   const uint8_t encoderPulsesPerStep = 2;  // Require 2 pulses for 1 menu step
   bool enableMenuWraparound = false;  // Disable wraparound by default
   
   // Screen saver
   unsigned long screenSaverTimeout = 0;  // 0 = disabled
   unsigned long lastActivityTime = 0;
   bool screenSaverActive = false;
   
   // Status screen and menu timeout
   unsigned long menuTimeout = 10000;  // 10 seconds default
   unsigned long lastMenuActivity = 0;
   StatusData* statusData = NULL;
   StatusRenderCallback statusRenderCallback = NULL;
   StatusInputCallback statusInputCallback = NULL;

   // Private methods
   void updateEncoder();
   ButtonEvent checkButtons();
   void renderStatusScreen();
   void renderMenuNavigation();
   void renderNumericValue();
   void renderStringComposition();
   void renderEitherOr();
   void enterMenuItem(MenuItem* item);
   void exitCurrentMode();
   MenuItem* getItemAtIndex(uint8_t index);
   uint8_t getMenuItemCount(MenuItem* menu);
   
   // ISR handlers (must be static)
   static void IRAM_ATTR encoderISR();
   static void IRAM_ATTR confirmISR();
   static void IRAM_ATTR backISR();
   static void IRAM_ATTR encoderSwISR();
   static MenuSystem* instance; // For ISR access
   
 public:
   MenuSystem(uint8_t width = 128, uint8_t height = 64, int8_t resetPin = -1);
   ~MenuSystem();
   
   // Initialization
   bool begin(uint8_t sdaPin, uint8_t sclPin, 
              uint8_t encClk, uint8_t encDt, 
              uint8_t btnConfirm, uint8_t btnBack,
              uint8_t encSw = 255);
   
   // Menu creation
   MenuItem* createMenu(const char* title);
   void setRootMenu(MenuItem* menu);
   
   // Add menu items
   MenuItem* addSubmenu(MenuItem* parent, const char* label, MenuItem* submenu);
   MenuItem* addAction(MenuItem* parent, const char* label, ActionCallback callback);
   MenuItem* addNumericValue(MenuItem* parent, const char* label, int* valuePtr, 
                            int minVal, int maxVal, int step = 1, const char* unit = "");
   MenuItem* addStringValue(MenuItem* parent, const char* label, char* buffer, 
                           uint8_t maxLen, const char* charset = NULL);
   MenuItem* addEitherOr(MenuItem* parent, const char* label, bool* valuePtr,
                        const char* opt1 = "YES", const char* opt2 = "NO");
   MenuItem* addInfo(MenuItem* parent, const char* label);
   
   // Main loop
   void update();
   
   // Display control
   void setContrast(uint8_t contrast);
   void enableScreenSaver(unsigned long timeoutMs);
   void disableScreenSaver();
   void setMenuTimeout(unsigned long timeoutMs);
   
   // Status screen
   void setStatusData(StatusData* data);
   void setStatusRenderCallback(StatusRenderCallback callback);
   void setStatusInputCallback(StatusInputCallback callback);
   void showStatusScreen();
   void showMenu();
   
   // Utility
   void navigateToMenu(MenuItem* menu);
   void resetActivity();  // Reset screen saver timer
   void resetMenuActivity();  // Reset menu timeout timer
 };
 
 #endif // ESP32_MENU_SYSTEM_H