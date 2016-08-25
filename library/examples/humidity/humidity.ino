#include <AM2302.h>


#define HUMIDITY_PIN 7
AM2302 humidityAm2302(HUMIDITY_PIN);

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Am2302 ready !");
}

void loop(void)
{
  AM2302_ERROR_TYPE status;
  char buf[128];

  humidityAm2302.reset();
  delay(2000);

  Serial.print("reading ... ");

  status = humidityAm2302.readData();
  switch (status) {
  case AM2302_ERROR_NONE:
    Serial.print("Sensor ");
    Serial.print(humidityAm2302.getTemperatureC());
    Serial.print("C ");
    Serial.print(humidityAm2302.getHumidity());
    Serial.println("%");

    sprintf(buf, "Value in Integer: Temperature %hi.%01hi C, Humidity %i.%01i %% RH",
            humidityAm2302.getTemperatureCInt() / 10, abs(humidityAm2302.getTemperatureCInt() % 10),
            humidityAm2302.getHumidityInt() / 10, humidityAm2302.getHumidityInt() % 10);
    Serial.println(buf);
    break;
  case AM2302_ERROR_CHECKSUM:
    Serial.print("check sum error ");
    Serial.print(humidityAm2302.getTemperatureC());
    Serial.print("C ");
    Serial.print(humidityAm2302.getHumidity());
    Serial.println("%");
    break;
  case AM2302_BUS_HUNG:
    Serial.println("One wire Hung ");
    break;
  case AM2302_ERROR_NOT_PRESENT:
    Serial.println("No Present");
    break;
  case AM2302_ERROR_ACK_TOO_LONG:
    Serial.println("No ACK ");
    break;
  case AM2302_ERROR_SYNC_TIMEOUT:
    Serial.println("No Sync ");
    break;
  case AM2302_ERROR_DATA_TIMEOUT:
    Serial.println("No data ");
    break;
  case AM2302_ERROR_TOOQUICK:
    Serial.println("Polling busy !");
    break;
  }
}