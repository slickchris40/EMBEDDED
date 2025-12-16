#ifndef WEB_SERVICE_H
#define WEB_SERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

class WebService {
  public:
    WebService(
      const char *ssid,
      const char *password,
      const char *serverUrl
    );

    void begin();                          
    void sendLog(const String &msg);      
    void sendGraph(int slots, bool dark);  

    bool isConnected();

  private:
    const char *_ssid;
    const char *_password;
    const char *_serverUrl;

    unsigned long _lastGraphSend;

    void postJSON(const String &json);
};

#endif
