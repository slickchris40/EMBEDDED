#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SmartParking.h"
#include "WebService.h"

/* ===== WIFI CREDENTIALS ===== */
#define WIFI_SSID     "DHVSU"
#define WIFI_PASSWORD "kissmuna"
#define SERVER_URL    "https://your-server-url/update"

/* ===== MODE PIN ===== */
#define MODE_PIN 25

/* ===== ENUM FOR SYSTEM MODES ===== */
enum SystemMode {
  MODE_SLEEP = 0,
  MODE_AUTO,
  MODE_MAINTENANCE
};

/* ===== LCD ===== */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* ===== OBJECTS ===== */
SmartParking parking(
  lcd,
  32, 33, 34,
  27, 14,
  35, 26
);

WebService web(
  WIFI_SSID,
  WIFI_PASSWORD,
  SERVER_URL
);

void setup() {
  Serial.begin(115200);

  pinMode(MODE_PIN, INPUT_PULLUP);

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  parking.begin();
  web.begin();
}

void loop() {

  /* ===== READ MODE ===== */
  SystemMode mode = (SystemMode)digitalRead(MODE_PIN);

  switch (mode) {

    /* ===== AUTO MODE ===== */
    case MODE_AUTO:
      parking.setMaintenance(false);
      parking.autoMode();

      web.sendGraph(
        parking.getSlotsLeft(),
        parking.isDark()
      );
      break;

    /* ===== MAINTENANCE MODE ===== */
    case MODE_MAINTENANCE:
      parking.setMaintenance(true);
      web.sendLog("Maintenance Mode Enabled");
      lcd.clear();
      lcd.print("MAINTENANCE");
      delay(1000);
      break;

    /* ===== SLEEP MODE ===== */
    case MODE_SLEEP:
    default:
      lcd.clear();
      lcd.print("SYSTEM SLEEP");
      delay(1000);
      esp_light_sleep_start();
      break;
  }

  delay(120);
}
