#pragma once

// Uses PlatformIO package ESP32 v6.9.0

// -------------------------------------------------------------------------------
// This files hold high level hardware configurations
// -------------------------------------------------------------------------------


// -------------------------------------------------------------------------------
#define SOFTWARE_VERSION "MCB6-ESP32 0.0.0.2"

// Add debug statments to code - needs extra space
#define DEBUG                       // DEBUG | DEBUG_OFF

// -------------------------------------------------------------------------------

// Define the type of OLED
#define OLED_SH1106                 // OLED_SH1106 (1.3") |  OLED_SSD1306 (0.96" and 2.4")

// -------------------------------------------------------------------------------

// If we output the digits reversed
#define NORMAL_DIGIT_OUTPUT        // REVERSE_DIGIT_OUTPUT | NORMAL_DIGIT_OUTPUT

// -------------------------------------------------------------------------------

#define FEATURE_MENU

// -------------------------------------------------------------------------------

#define CLOCK_MENU_TITLE "Burner6" 

