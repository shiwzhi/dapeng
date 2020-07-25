#include "run_dht.h"

DHT dht(D2, DHT11);

int dht_vcc = D1;

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
    hum = dht.readHumidity();
    temp = dht.readTemperature();
    uint dht_retry = 0;
    while (isnan(temp) || isnan(hum))
    {
        delay(100);
        hum = dht.readHumidity();
        temp = dht.readTemperature();
        dht_retry++;
        if (dht_retry > 50)
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
