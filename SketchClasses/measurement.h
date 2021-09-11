/*
  AnalogMeasurement.h - Library for reading analog input and processing the data.
  Created by Jolan Samwel.
  Released into the public domain.
*/

#ifndef measurement_h
#define measurement_h

#include "Arduino.h"

class analogSignal
{
  public:
    analogSignal(int pin);
    float readData();
  private:  
    static const int _bufferSize = 10;
  
    int _pin;
    int _analogInput;
    int _measurementBuffer[_bufferSize];

    float _reading;
};

#endif
