#include <Arduino.h>
#include "run_wifi.h"
#include "run_dht.h"
#include "run_ota.h"
#include "run_upload.h"

String version = "2.0.0";
ADC_MODE(ADC_VCC);

ulong start_time;

void goSleepSec(int sec)
{
    Serial.print("Running time: ");
    Serial.println(String((millis() - start_time) / 1000) + "s");
    Serial.println("go sleep for " + String(sec));
    ESP.deepSleep(sec * 1000 * 1000);
}

void setup()
{
    setup_dht();
    start_time = millis();

    Serial.begin(115200);

    // uint16 vcc = ESP.getVcc();
    // Serial.println(vcc);
    // if (vcc < 2800)
    // {
    //     Serial.println("deep sleep max");
    //     ESP.deepSleep(ESP.deepSleepMax());
    // }

    if (!run_wifi())
    {
        goSleepSec(5 * 60);
    }

    if (!run_dht())
    {
        goSleepSec(5 * 60);
    }

    if (!upload_tcp(readTemp(), readHum(), version.c_str(), "DHT11"))
    {
        goSleepSec(5 * 60);
    }

    if (read_ota())
    {
        run_ota(version.c_str());
    }

    goSleepSec(read_sleep() * 60);
}

void loop()
{
    if (millis() - start_time > 60 * 1000)
    {
        goSleepSec(5 * 60);
    }
    delay(1000);
}