#include "SmartParking.h"

/* ===== CONSTANTS ===== */
#define TOTAL_SLOTS 3
#define IR_CONFIRM_MS 250
#define STARTUP_IGNORE 4000
#define GATE_DISTANCE_CM 8
#define REQUIRED_HITS 3
#define GATE_DELAY 5000
#define GATE_COOLDOWN 3000
#define DARK_THRESHOLD 1800

static const uint8_t UNUSED_PINS[] = {
  0, 2, 4, 5, 12, 13, 15,
  16, 17, 18, 19,
  23, 25,
  36, 39
};

SmartParking::SmartParking(
  LiquidCrystal_I2C &lcdRef,
  uint8_t ir1, uint8_t ir2, uint8_t ir3,
  uint8_t trig, uint8_t echo,
  uint8_t ldr, uint8_t sensorPwr
) {
  _lcd = &lcdRef;
  _irPin[0] = ir1;
  _irPin[1] = ir2;
  _irPin[2] = ir3;
  _trigPin = trig;
  _echoPin = echo;
  _ldrPin = ldr;
  _sensorPwr = sensorPwr;

  _slotsLeft = TOTAL_SLOTS;
  _maintenance = false;
  _gateOpen = false;
  _detectCount = 0;
  _isDark = false;
  _lastGateEvent = 0;

  memset(_slot, 0, sizeof(_slot));
  memset(_slotStart, 0, sizeof(_slotStart));
  memset(_irLowSince, 0, sizeof(_irLowSince));
  memset(_irStable, 0, sizeof(_irStable));
}

void SmartParking::begin() {

  for (uint8_t pin : UNUSED_PINS) {
    if (pin >= 34 && pin <= 39) pinMode(pin, INPUT);
    else pinMode(pin, INPUT_PULLDOWN);
  }

  pinMode(_sensorPwr, OUTPUT);
  digitalWrite(_sensorPwr, HIGH);

  pinMode(_irPin[0], INPUT_PULLUP);
  pinMode(_irPin[1], INPUT_PULLUP);
  pinMode(_irPin[2], INPUT);

  pinMode(_trigPin, OUTPUT);
  pinMode(_echoPin, INPUT);
  pinMode(_ldrPin, INPUT);

  showMainLCD();
}

/* ===== AUTO MODE ===== */
void SmartParking::autoMode() {
  if (_maintenance) return;
  updateSlots();
  updateGate();
  updateLDR();
}

/* ===== SLOT LOGIC ===== */
bool SmartParking::readIRStable(uint8_t i) {
  if (millis() < STARTUP_IGNORE) return false;

  int state = (_irPin[i] == 34)
              ? safeDigitalRead(_irPin[i])
              : digitalRead(_irPin[i]);

  if (state == LOW) {
    if (_irLowSince[i] == 0) _irLowSince[i] = millis();
    if (millis() - _irLowSince[i] >= IR_CONFIRM_MS) _irStable[i] = true;
  } else {
    _irLowSince[i] = 0;
    _irStable[i] = false;
  }
  return _irStable[i];
}

void SmartParking::updateSlots() {
  for (int i = 0; i < 3; i++) {
    bool cur = readIRStable(i);
    if (cur && !_slot[i]) {
      _slot[i] = true;
      _slotsLeft--;
      showMainLCD();
    }
    if (!cur && _slot[i]) {
      _slot[i] = false;
      _slotsLeft++;
      showMainLCD();
    }
  }
}

/* ===== GATE ===== */
void SmartParking::updateGate() {
  digitalWrite(_trigPin, LOW); delayMicroseconds(2);
  digitalWrite(_trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(_trigPin, LOW);

  long echo = pulseIn(_echoPin, HIGH, 25000);
  if (echo <= 0) return;

  int dist = echo * 0.034 / 2;
  if (dist <= GATE_DISTANCE_CM) _detectCount++;
  else _detectCount = 0;

  if (_detectCount >= REQUIRED_HITS && !_gateOpen) {
    _gateOpen = true;
    _lastGateEvent = millis();
    showGateLCD("OPEN");
  }

  if (_gateOpen && millis() - _lastGateEvent > GATE_DELAY) {
    _gateOpen = false;
    showGateLCD("CLOSED");
  }
}

/* ===== LDR ===== */
void SmartParking::updateLDR() {
  _isDark = analogRead(_ldrPin) < DARK_THRESHOLD;
}

/* ===== LCD ===== */
void SmartParking::showMainLCD() {
  _lcd->clear();
  _lcd->print("Slots Left: ");
  _lcd->print(_slotsLeft);
}

void SmartParking::showGateLCD(const char *msg) {
  _lcd->clear();
  _lcd->print(msg);
  delay(1000);
  showMainLCD();
}

/* ===== HELPERS ===== */
int SmartParking::safeDigitalRead(uint8_t pin) {
  int s = 0;
  for (int i = 0; i < 10; i++) s += digitalRead(pin);
  return (s > 5) ? HIGH : LOW;
}

int SmartParking::getSlotsLeft() { return _slotsLeft; }
bool SmartParking::isDark() { return _isDark; }
void SmartParking::setMaintenance(bool on) { _maintenance = on; }
