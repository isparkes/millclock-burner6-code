#include "WebManager.h"

// This include has to be here, anbd not in the header file
#include <ElegantOTA.h>

// I had to fiddle in the ElegantOTA source to get this to work
// Line 27: #define ELEGANTOTA_USE_ASYNC_WEBSERVER 1
// #define ELEGANTOTA_USE_ASYNC_WEBSERVER

// ************************************************************
// Open up the normal page handlers
// ************************************************************
void WebManager_::begin() {
  debugMsgWbm("Setting up server endpoints");
  server.reset();
  server.serveStatic("/", SPIFFS, "/web/").setDefaultFile("index.html");

  // Summary and diagnostics
  server.on("/api/getSummary", HTTP_GET, getSummaryDataHandler);
  server.on("/api/getDiags", HTTP_GET, getDiagsDataHandler);

 // Configure options
  server.on("/api/getConfig", HTTP_GET, getConfigDataHandler);
  server.on("/api/postConfig", HTTP_POST, postConfigDataHandler);

  // wifi credentials
  server.on("/api/postWiFiCredentials", HTTP_POST, postWiFiCredentialsHandler);
  server.on("/api/credentials", HTTP_GET, getCredentialsHandler);

  // Counter control
  server.on("/api/startCounter", HTTP_GET, startCounterHandler);
  server.on("/api/stopCounter", HTTP_GET, stopCounterHandler);
  server.on("/api/resetCounter", HTTP_GET, resetCounterHandler);
  server.on("/api/pauseCounter", HTTP_GET, pauseCounterHandler);
  server.on("/api/counterStatus", HTTP_GET, counterStatusHandler);

  // Utilities
  server.on("/utils/resetwifi", HTTP_GET, resetWifiHandler);
  server.on("/utils/scanI2C", HTTP_GET, getI2CScanHandler);
  server.on("/utils/scanSPIFFS", HTTP_GET, getSPIFFSScanHandler);
  server.on("/utils/saveStats", HTTP_GET, saveStatsHandler);
  server.on("/utils/resetoptions", HTTP_GET, [] (AsyncWebServerRequest *request) {
    resetOptions();
        request->redirect("/utility.html");;
    });
  server.on("/utils/resetall", HTTP_GET, [] (AsyncWebServerRequest *request) {
    resetAll();
        request->redirect("/utility.html");;
    });
  server.on("/utils/restart", HTTP_GET, restartHandler);

  server.onNotFound([](AsyncWebServerRequest *request){
      request->send(404, "text/plain", "The content you are looking for was not found.");
  });

  debugMsgWbm("Start up web server");

  server.begin();
}

// ************************************************************
// Handler for the captive page
// ************************************************************
class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    debugMsgWbm("canHandle URL: " + request->url());
    if (request->url().startsWith("/api/")) {
      debugMsgWbm("Skipping - API route");
      return false;
    }
    if (request->url().startsWith("/utils/")) {
      debugMsgWbm("Skipping - utils route");
      return false;
    }
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    debugMsgWbm("Sending captive page for: " + request->url());
    if (SPIFFS.exists("/web/portal.html")) {
      debugMsgWbm("portal.html exists, sending...");
      request->send(SPIFFS, "/web/portal.html", "text/html", false);
    } else {
      debugMsgWbm("ERROR: portal.html not found in SPIFFS!");
      request->send(500, "text/plain", "Portal page not found");
    }
  }
};

// ************************************************************
// Open up the Portal Page
// ************************************************************
void WebManager_::beginPortal() {
  debugMsgWbm("=== beginPortal() called ===");
  server.reset();

  // Serve portal page at root
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    debugMsgWbm("Root path requested - serving portal");
    if (SPIFFS.exists("/web/portal.html")) {
      request->send(SPIFFS, "/web/portal.html", "text/html");
    } else {
      request->send(500, "text/plain", "portal.html not found");
    }
  });

  // Simple test endpoint
  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request) {
    debugMsgWbm("TEST endpoint hit!");
    request->send(200, "text/plain", "Server is working!");
  });

  // wifi credentials and scanning
  server.on("/api/postWiFiCredentials", HTTP_POST, postWiFiCredentialsHandler);
  server.on("/api/credentials", HTTP_GET, getCredentialsHandler);
  server.on("/api/getWiFiNetworks", HTTP_GET, getWiFiNetworksHandler);
  server.on("/api/scanWiFi", HTTP_GET, [](AsyncWebServerRequest *request) {
    debugMsgWbm("Starting WiFi scan...");
    wifiManager.startScanWiFiNetworks();
    request->send(200, "text/json", "{\"status\": \"scanning\"}");
  });

  // Utilities
  server.on("/utils/resetwifi", HTTP_GET, resetWifiHandler);
  server.on("/utils/scanI2C", HTTP_GET, getI2CScanHandler);
  server.on("/utils/scanSPIFFS", HTTP_GET, getSPIFFSScanHandler);
  server.on("/utils/saveStats", HTTP_GET, saveStatsHandler);

  // Catch-all for captive portal redirect (any other path)
  server.onNotFound([](AsyncWebServerRequest *request) {
    debugMsgWbm("Not found: " + request->url() + " - redirecting to portal");
    request->redirect("/");
  });

  // All your DNS requests are belong to us
  wifiManager.startDNSD();

  debugMsgWbm("Start up web server");

  server.begin();
}

// ************************************************************
// Start the OTA service
// ************************************************************
void WebManager_::startOTA() {
  ElegantOTA.begin(&server, "admin", "update");
}

// ************************************************************
// Get singleton instance
// ************************************************************
WebManager_ &WebManager_::getInstance() {
  static WebManager_ instance;
  return instance;
}

WebManager_ &webManager = webManager.getInstance();