#include "WebService.h"

WebService::WebService(
  const char *ssid,
  const char *password,
  const char *serverUrl
) {
  _ssid = ssid;
  _password = password;
  _serverUrl = serverUrl;
  _lastGraphSend = 0;
}

void WebService::begin() {
  WiFi.begin(_ssid, _password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
  }
}

bool WebService::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void WebService::sendLog(const String &msg) {
  if (!isConnected()) return;

  String json =
    "{"
    "\"type\":\"log\","
    "\"message\":\"" + msg + "\""
    "}";

  postJSON(json);
}

void WebService::sendGraph(int slots, bool dark) {
  if (!isConnected()) return;
  if (millis() - _lastGraphSend < 5000) return;

  String json =
    "{"
    "\"type\":\"graph\","
    "\"slots\":" + String(slots) + ","
    "\"light\":\"" + String(dark ? "dark" : "light") + "\""
    "}";

  postJSON(json);
  _lastGraphSend = millis();
}

void WebService::postJSON(const String &json) {
  HTTPClient http;
  http.begin(_serverUrl);
  http.addHeader("Content-Type", "application/json");
  http.POST(json);
  http.end();
}
