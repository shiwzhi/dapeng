#include "run_dht.h"

DHT dht(13, DHT11);

int dht_vcc = 12;

float hum = 0;
float temp = 0;

void setup_dht()
{
    pinMode(dht_vcc, OUTPUT);
    digitalWrite(dht_vcc, HIGH);
    dht.begin();
}

bool run_dht()
{
    delay(1000);
    hum = dht.readHumidity();
    temp = dht.readTemperature();
    uint dht_retry = 0;
    while (isnan(temp) || isnan(hum))
    {
        delay(1000);
        hum = dht.readHumidity();
        temp = dht.readTemperature();
        dht_retry++;
        if (dht_retry > 20)
        {
            Serial.println("DHT Failed");
            return false;
        }
    }
    Serial.println("temp:" + String(temp));
    Serial.println("hum:" + String(hum));
    return true;
}

float readTemp()
{
    return temp;
}

float readHum()
{
    return hum;
}
