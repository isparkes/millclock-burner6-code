// ************************************************************
// Global types used to hold the config and statistics
// ************************************************************

#pragma once

#include <Arduino.h>

// ------------------------ Types ------------------------

// Used for holding the config set
typedef struct {
  bool webAuthentication;
  String webUsername;
  String webPassword;
  String WiFiSSID;
  String WiFiPassword;
  bool WifiOnAtStart;

  bool testMode;
  bool wasSetup;
} spiffs_config_t;

typedef struct {
  unsigned long uptimeMins = 0;
  unsigned long tubeOnTimeMins = 0;
} spiffs_stats_t;

