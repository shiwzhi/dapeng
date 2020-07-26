#include "run_sht30.h"

SHTSensor sht;

float temp = 0;
float hum = 0;

bool run_sht()
{
    Wire.begin(4, 5);
    if (sht.init())
    {
        Serial.print("sht init(): success\n");
    }
    else
    {
        Serial.print("sht init(): failed\n");
        return false;
    }
    sht.setAccuracy(SHTSensor::SHT_ACCURACY_HIGH);

    if (sht.readSample())
    {
        
        Serial.print("SHT:\n");
        Serial.print("  RH: ");
        Serial.print(sht.getHumidity(), 2);
        Serial.print("\n");
        Serial.print("  T:  ");
        Serial.print(sht.getTemperature(), 2);
        Serial.print("\n");

        temp = sht.getTemperature();
        hum = sht.getHumidity();
        return true;
    }
    else
    {
        Serial.print("Error in readSample()\n");
        return false;
    }
}

float readTemp() {
    return temp;
}

float readHum() {
    return hum;
}