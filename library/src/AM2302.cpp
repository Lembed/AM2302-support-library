/*
  AM2302.cpp - AM2302 sensor library
  License: MIT
*/

#include "AM2302.h"
#include <pins_arduino.h>

// This should be 40, but the sensor is adding an extra bit at the start
#define AM2302_DATA_BIT_COUNT 41

AM2302::AM2302(uint8_t pin)
{
  _bitmask = digitalPinToBitMask(pin);
  _baseReg = portInputRegister(digitalPinToPort(pin));
  _lastReadTime = millis();
  _lastHumidity = AM2302_ERROR_VALUE;
  _lastTemperature = AM2302_ERROR_VALUE;
}

void AM2302::disableInterrupt()
{
  noInterrupts();
}

void AM2302::enableInterrupt()
{
  interrupts();
}

bool AM2302::pinRead()
{
  return ((*(_baseReg)) & (_bitmask)) ? 1 : 0;
}

bool AM2302::pinSetInput()
{
  return (*(_baseReg + 1)) &= ~(_bitmask);
}

bool AM2302::pinSetOutput()
{
  return (*(_baseReg + 1)) |= (_bitmask);
}

bool AM2302::pinPullDown()
{
  return  (*(base + 2)) &= ~(mask);
}


void AM2302::findPluse(int loopcount, AM2302_ERROR_TYPE status)
{
  retryCount = 0;
  do {
    if (retryCount > loopcount) {
      return status;
    }
    retryCount++;
    delayMicroseconds(2);
  } while (!pinRead());
}

DHT22_ERROR_TYPE AM2302::readData()
{
  uint8_t retryCount;
  uint8_t bitTimes[AM2302_DATA_BIT_COUNT];
  int currentHumidity;
  int currentTemperature;
  uint8_t checkSum, csPart1, csPart2, csPart3, csPart4;
  unsigned long currentTime;
  int i;

  currentHumidity = 0;
  currentTemperature = 0;
  checkSum = 0;
  currentTime = millis();
  for (i = 0; i < AM2302_DATA_BIT_COUNT; i++) {
    bitTimes[i] = 0;
  }

  if (currentTime - _lastReadTime < 2000) {
    // Caller needs to wait 2 seconds between each call to readData
    return AM2302_ERROR_TOOQUICK;
  }
  _lastReadTime = currentTime;

  // Pin needs to start HIGH, wait until it is HIGH with a timeout
  disableInterrupt();
  pinSetInput();
  enableInterrupt();

  findPluse(125, AM2302_BUS_HUNG);

  // Send the activate pulse
  disableInterrupt();

  pinPullDown();
  pinSetOutput();

  enableInterrupt();
  delayMicroseconds(1100); // 1.1 ms
  disableInterrupt();

  // Switch back to input so pin can float
  pinSetInput();

  enableInterrupt();

  // Find the start of the ACK Pulse
  // Spec is 20 to 40 us, 25*2 == 50 us
  findPluse(25, AM2302_ERROR_NOT_PRESENT);

  // Find the end of the ACK Pulse
  // Spec is 80 us, 50*2 == 100 us
  findPluse(50, AM2302_ERROR_ACK_TOO_LONG);

  // Read the 40 bit data stream
  for (i = 0; i < AM2302_DATA_BIT_COUNT; i++) {

    // Find the start of the sync pulse
    // Spec is 50 us, 35*2 == 70 us
    findPluse(50, AM2302_ERROR_SYNC_TIMEOUT);

    // Measure the width of the data pulse
    // Spec is 80 us, 50*2 == 100 us
    findPluse(50, AM2302_ERROR_DATA_TIMEOUT);

    bitTimes[i] = retryCount;
  }

  // Now bitTimes have the number of retries (us *2)
  // that were needed to find the end of each data bit
  // Spec: 0 is 26 to 28 us
  // Spec: 1 is 70 us
  // bitTimes[x] <= 11 is a 0
  // bitTimes[x] >  11 is a 1
  // Note: the bits are offset by one from the data sheet, not sure why
  for (i = 0; i < 16; i++) {
    if (bitTimes[i + 1] > 11) {
      currentHumidity |= (1 << (15 - i));
    }
  }
  for (i = 0; i < 16; i++) {
    if (bitTimes[i + 17] > 11) {
      currentTemperature |= (1 << (15 - i));
    }
  }
  for (i = 0; i < 8; i++) {
    if (bitTimes[i + 33] > 11) {
      checkSum |= (1 << (7 - i));
    }
  }

  _lastHumidity = currentHumidity & 0x7FFF;
  if (currentTemperature & 0x8000) {
    // Below zero, non standard way of encoding negative numbers!
    // Convert to native negative format.
    _lastTemperature = -(currentTemperature & 0x7FFF);
  } else {
    _lastTemperature = currentTemperature;
  }

  csPart1 = currentHumidity >> 8;
  csPart2 = currentHumidity & 0xFF;
  csPart3 = currentTemperature >> 8;
  csPart4 = currentTemperature & 0xFF;
  if (checkSum == ((csPart1 + csPart2 + csPart3 + csPart4) & 0xFF)) {
    return AM2302_ERROR_NONE;
  }

  return AM2302_ERROR_CHECKSUM;
}



short int AM2302::getHumidityInt()
{
  return _lastHumidity;
}

short int AM2302::getTemperatureCInt()
{
  return _lastTemperature;
}


float AM2302::getHumidity()
{
  return float(_lastHumidity) / 10;
}


float AM2302::getTemperatureC()
{
  return float(_lastTemperature) / 10;
}

float AM2302::getTemperatureF()
{
  return float(_lastTemperature) / 10 * 9 / 5 + 32;
}

void AM2302::clockReset()
{
  _lastReadTime = millis();
}
