/*
  AnalogMeasurement.h - Library for reading analog input and processing the data.
  Created by Jolan Samwel.
  Released into the public domain.
*/

#include "Arduino.h"
#include "measurement.h"

analogSignal::analogSignal(int pin)
{
  pinMode(pin, INPUT);
  _pin = pin;
}

// reads analog signal, filters it to reduce noice and scales the output
float analogSignal::readData()
{
  float average = 0;
  int i;

  // shift elements in buffer to make room for new measurement
  // add each element in the buffer to average
  for (int i = _bufferSize - 1; i > 0; i--) {
    _measurementBuffer[i] = _measurementBuffer[i - 1];
    average += _measurementBuffer[i];
  }

  // get current value input
  _reading = analogRead(_pin);

  // add latest measurement to buffer and average
  _measurementBuffer[0] = _reading;
  average += _reading;

  // calculate and return average
  return map(average /= _bufferSize,0,1024,100,0);
}
