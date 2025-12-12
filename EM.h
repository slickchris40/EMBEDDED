#ifndef EM_H
#define EM_H

#include <Arduino.h>
#include <ESP32Servo.h>

class EM {
  public:
    EM(int ir1, int ir2, int ir3,
       int trig, int echo,
       int servoPin, int ldrPin);

    void begin();

    bool slot1();
    bool slot2();
    bool slot3();

    long distance();

    void gateOpen();
    void gateClose();

    int readLDR();

  private:
    int _ir1, _ir2, _ir3;
    int _trig, _echo;
    int _servoPin;
    int _ldrPin;

    Servo _servo;
};

#endif
