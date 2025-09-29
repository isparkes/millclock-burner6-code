#include "SpiffsStorage.h"

//**********************************************************************************
//**********************************************************************************
//*                               SPIFFS functions                                 *
//**********************************************************************************
//**********************************************************************************
// ************************************************************
// Test SPIFFS
// ************************************************************
bool SpiffsStorage_::testMountSpiffs()
{
  bool mounted = false;
  if (SPIFFS.begin())
  {
    mounted = true;
  }
  return mounted;
}

// ************************************************************
// Retrieve the config from the SPIFFS
// ************************************************************
bool SpiffsStorage_::getConfigFromSpiffs()
{
  bool loaded = false;
  debugMsgSpfX("mounted file system config read");
  if (SPIFFS.exists("/config/config.json"))
  {
    // file exists, reading and loading
    debugMsgSpf("Reading config file");
    File configFile = SPIFFS.open("/config/config.json", "r");
    if (configFile)
    {
      debugMsgSpfX("opened config file");
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      DynamicJsonBuffer jsonBuffer;
      JsonObject &json = jsonBuffer.parseObject(buf.get());
      #ifdef SPF_EXTENDED_DEBUG
      // Dump the raw JSON
      json.printTo(Serial);
      debugMsgSpfX("\n");
      #endif
      if (json.success())
      {
        debugMsgSpfX("parsed config json");

        cc->webAuthentication = json["webAuthentication"].as<bool>();
        debugMsgSpfX("Loaded webAuthentication: " + String(cc->webAuthentication));

        cc->webUsername = json["webUsername"].as<String>();
        debugMsgSpfX("Loaded webUsername: " + cc->webUsername);

        cc->webPassword = json["webPassword"].as<String>();
        debugMsgSpfX("Loaded webPassword: " + cc->webPassword);

        cc->WiFiSSID = json["WiFiSSID"].as<String>();
        debugMsgSpfX("Loaded WiFiSSID: " + String(cc->WiFiSSID));

        cc->WiFiPassword = json["WiFiPassword"].as<String>();
        debugMsgSpfX("Loaded WiFiPassword: " + String(cc->WiFiPassword));

        cc->WifiOnAtStart = json["WifiOnAtStart"].as<bool>();
        debugMsgSpfX("Loaded WifiOnAtStart: " + String(cc->WifiOnAtStart));

        cc->counterValuesZIN70 = json["counterValuesZIN70"].as<String>();
        debugMsgSpfX("Loaded counterValues ZIN70: " + cc->counterValuesZIN70);

        cc->counterValuesZIN18 = json["counterValuesZIN18"].as<String>();
        debugMsgSpfX("Loaded counterValues ZIN18: " + cc->counterValuesZIN18);

        cc->tubeType = json["tubeType"];
        debugMsgSpfX("Loaded tubeType: " + String(cc->tubeType));

        cc->tubeBoardCount = json["tubeBoardCount"];
        debugMsgSpfX("Loaded tubeBoardCount: " + String(cc->tubeBoardCount));

        loaded = true;
      }
      else
      {
        debugMsgSpf("failed to load json config");
      }
      debugMsgSpfX("Closing config file");

      configFile.close();
    }
  }
  return loaded;
}

// ************************************************************
// Save config back to the SPIFFS
// ************************************************************
void SpiffsStorage_::saveConfigToSpiffs()
{
  debugMsgSpf("Saving config");

  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();
  json["webAuthentication"] = cc->webAuthentication;
  json["webUsername"] = cc->webUsername;
  json["webPassword"] = cc->webPassword;
  json["WiFiSSID"] = cc->WiFiSSID;
  json["WiFiPassword"] = cc->WiFiPassword;
  json["WifiOnAtStart"] = cc->WifiOnAtStart;
  json["counterValuesZIN70"] = cc->counterValuesZIN70;
  json["counterValuesZIN18"] = cc->counterValuesZIN18;
  json["tubeType"] = cc->tubeType;
  json["tubeBoardCount"] = cc->tubeBoardCount;
  
  File configFile = SPIFFS.open("/config/config.json", "w");
  if (!configFile)
  {
    debugMsgSpf("Failed to open config file for writing");

    configFile.close();
    return;
  }
  json.printTo(configFile);
  configFile.close();
  debugMsgSpf("Saved config");
}

// ************************************************************
// Get the statistics from the SPIFFS
// ************************************************************
bool SpiffsStorage_::getStatsFromSpiffs()
{
  bool loaded = false;
  if (SPIFFS.exists("/config/stats.json"))
  {
    // file exists, reading and loading
    debugMsgSpf("Reading stats file");

    File statsFile = SPIFFS.open("/config/stats.json", "r");
    if (statsFile)
    {
      debugMsgSpfX("opened stats file");

      size_t size = statsFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      statsFile.readBytes(buf.get(), size);
      DynamicJsonBuffer jsonBuffer;
      JsonObject &json = jsonBuffer.parseObject(buf.get());
      if (json.success())
      {
        debugMsgSpfX("parsed stats json");

        cs->uptimeMins = json.get<unsigned long>("uptime");
        debugMsgSpfX("Loaded uptime: " + String(cs->uptimeMins));

        cs->tubeOnTimeMins = json.get<unsigned long>("tubeontime");
        debugMsgSpfX("Loaded tubeontime: " + String(cs->tubeOnTimeMins));

        loaded = true;
      }
      else
      {
        debugMsgSpf("Failed to load json config");
      }
      debugMsgSpfX("Closing stats file");

      statsFile.close();
    }
  }
  return loaded;
}

// ************************************************************
// Save the statistics back to the SPIFFS
// ************************************************************
void SpiffsStorage_::saveStatsToSpiffs()
{
  debugMsgSpf("Saving stats");
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();
  json.set("uptime", cs->uptimeMins);
  json.set("tubeontime", cs->tubeOnTimeMins);
  File statsFile = SPIFFS.open("/config/stats.json", "w");
  if (!statsFile)
  {
    debugMsgSpf("Failed to open stats file for writing");
    statsFile.close();
    return;
  }
  json.printTo(statsFile);
  statsFile.close();
  debugMsgSpf("Saved stats");
}


// ************************************************************
// Internal plumbing
// ************************************************************

SpiffsStorage_ &SpiffsStorage_::getInstance() {
  static SpiffsStorage_ instance;
  return instance;
}

SpiffsStorage_ &spiffsStorage = spiffsStorage.getInstance();