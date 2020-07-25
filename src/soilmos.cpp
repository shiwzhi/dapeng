#include "soilmois.h"
#include <Arduino.h>

void setup_soil()
{
    int soil_vcc = 4;
    pinMode(soil_vcc, OUTPUT);
    digitalWrite(soil_vcc, HIGH);
}

float get_soilMois()
{
    float soil_reading = analogRead(A0);
    soil_reading = map(soil_reading, 190, 320, 100, 0); 
    Serial.println(soil_reading);
    return soil_reading;
}