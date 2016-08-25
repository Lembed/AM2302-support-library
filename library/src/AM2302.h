#ifndef __AM2302_H__
#define __AM2302_H__


#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <inttypes.h>
#include <stdio.h>

// Setup debug printing macros.
#ifdef AM2302_DEBUG
#define DEBUG_PRINT(...) { Serial.print(__VA_ARGS__); }
#define DEBUG_PRINTLN(...) { Serial.println(__VA_ARGS__); }
#else
#define DEBUG_PRINT(...) {}
#define DEBUG_PRINTLN(...) {}
#endif

#define AM2302_ERROR_VALUE -995

typedef enum {
  AM2302_ERROR_NONE = 0,
  AM2302_BUS_HUNG,
  AM2302_ERROR_NOT_PRESENT,
  AM2302_ERROR_ACK_TOO_LONG,
  AM2302_ERROR_SYNC_TIMEOUT,
  AM2302_ERROR_DATA_TIMEOUT,
  AM2302_ERROR_CHECKSUM,
  AM2302_ERROR_TOOQUICK
} AM2302_ERROR_TYPE;

class AM2302
{
private:
  uint8_t _bitmask;
  volatile uint8_t *_baseReg;
  unsigned long _lastReadTime;
  short int _lastHumidity;
  short int _lastTemperature;

public:
  AM2302(uint8_t pin);

  /**
   * @brief      Read the 40 bit data stream from the AM2302
   * Store the results in private member data to be read by public member functions
   *
   * @return     one wire status
   */
  AM2302_ERROR_TYPE readData();

  /**
   * @brief  Report the humidity in .1 percent increments, such that 635 means 63.5% relative humidity
   * Converts from the internal integer format on demand, so you might want to cache the result.
   *
   * @return     The humidity value in integer format.
   */
  short int getHumidityInt();

  /**
   * @brief     Get the temperature in decidegrees C, such that 326 means 32.6 degrees C.
   * The temperature may be negative, so be careful when handling the fractional part.
   *
   * @return     The temperature int integer format .
   */
  short int getTemperatureCInt();

  /**
   * @brief This is used when the millis clock rolls over to zero
   */
  void clockReset();

  /**
   * @brief Return the percentage relative humidity in decimal form
   */
  float getHumidity();

  /**
    * @brief Return the percentage relative humidity in decimal form
    * Converts from the internal integer format on demand, so you might want
    * to cache the result.
    */
  float getTemperatureC();

  /**
   * @brief      Gets the temperature f.
   *
   * @return     The temperature f.
   */
  float getTemperatureF();

private:

  /**
   * @brief      Disables the interrupt.
   */
  void disableInterrupt();

  /**
   * @brief      Enables the interrupt.
   */
  void enableInterrupt();

  /**
   * @brief      count the pluses and return the one wire status
   *
   * @param[in]  loopcount  The loopcount
   * @param[in]  status     The status
   */
  void findPluse(int loopcount, AM2302_ERROR_TYPE status);

  /**
   * @brief      get the status of the digital pin
   *
   * @return     true
   */
  bool pinRead();

  /**
   * @brief      set the digital pin to input mode
   *
   * @return     true
   */
  bool pinSetInput();

  /**
   * @brief      set the digital pin to output mode
   *
   * @return     true
   */
  bool pinSetOutput();

  /**
   * @brief      pull the digital pin to low
   *
   * @return     true
   */
  bool pinPullDown();

};


#endif /*__AM2302_H__*/
