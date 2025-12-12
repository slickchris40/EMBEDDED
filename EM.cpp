#include "EM.h"

EM::EM(int ir1, int ir2, int ir3,
       int trig, int echo,
       int servoPin, int ldrPin) {

  _ir1 = ir1;
  _ir2 = ir2;
  _ir3 = ir3;
  _trig = trig;
  _echo = echo;
  _servoPin = servoPin;
  _ldrPin = ldrPin;
}

void EM::begin() {
  pinMode(_ir1, INPUT);
  pinMode(_ir2, INPUT);
  pinMode(_ir3, INPUT);

  pinMode(_trig, OUTPUT);
  pinMode(_echo, INPUT);

  pinMode(_ldrPin, INPUT);

  _servo.attach(_servoPin);
  gateClose();
}

// IR sensor: LOW = OBJECT DETECTED = FULL
bool EM::slot1() { return digitalRead(_ir1) == LOW; }
bool EM::slot2() { return digitalRead(_ir2) == LOW; }
bool EM::slot3() { return digitalRead(_ir3) == LOW; }

long EM::distance() {
  digitalWrite(_trig, LOW);
  delayMicroseconds(2);
  digitalWrite(_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trig, LOW);

  long duration = pulseIn(_echo, HIGH, 30000);
  return duration * 0.034 / 2;
}

void EM::gateOpen() {
  _servo.write(90);
}

void EM::gateClose() {
  _servo.write(0);
}

int EM::readLDR() {
  return analogRead(_ldrPin);
}
