#ifndef SMART_PARKING_H
#define SMART_PARKING_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class SmartParking {
  public:
    SmartParking(
      LiquidCrystal_I2C &lcdRef,
      uint8_t ir1, uint8_t ir2, uint8_t ir3,
      uint8_t trig, uint8_t echo,
      uint8_t ldr, uint8_t sensorPwr
    );

    void begin();
    void autoMode();               // AUTO MODE
    void setMaintenance(bool on);  // MAINTENANCE MODE

    int getSlotsLeft();            // for website
    bool isDark();                 // for graph

  private:
    /* ===== LCD ===== */
    LiquidCrystal_I2C *_lcd;

    /* ===== PINS ===== */
    uint8_t _irPin[3];
    uint8_t _trigPin;
    uint8_t _echoPin;
    uint8_t _ldrPin;
    uint8_t _sensorPwr;

    /* ===== STATES ===== */
    bool _slot[3];
    unsigned long _slotStart[3];
    int _slotsLeft;
    bool _maintenance;
    bool _isDark;

    /* ===== GATE ===== */
    bool _gateOpen;
    unsigned long _lastGateEvent;
    int _detectCount;

    /* ===== IR FILTER ===== */
    unsigned long _irLowSince[3];
    bool _irStable[3];

    /* ===== INTERNAL ===== */
    bool readIRStable(uint8_t index);
    int safeDigitalRead(uint8_t pin);
    void updateSlots();
    void updateGate();
    void updateLDR();
    void showMainLCD();
    void showGateLCD(const char *msg);
};

#endif
