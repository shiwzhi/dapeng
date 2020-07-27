#include "run_soilMois.h"

int soil_vcc = 13;

void setup_soilMois()
{
    pinMode(soil_vcc, OUTPUT);
    digitalWrite(soil_vcc, HIGH);
}

float read_soilMois()
{
    float reading;
    for (int i = 0; i < 10; i++)
    {
        reading = analogRead(A0);
        // Serial.println(reading);
        reading = map(reading, 0, 1023, 0, 1000);
        reading = map(reading, 327, 169, 0, 100);
        // Serial.println("Soil mois:" + String(reading));
        delay(100);
    }

    return reading;
}