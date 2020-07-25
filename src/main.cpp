#include <Arduino.h>
#include "run_wifi.h"
#include "run_dht.h"
#include "run_ota.h"
#include "run_upload.h"
#include "soilmois.h"

String version = "2.0.1";
// ADC_MODE(ADC_VCC);

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
    setup_soil();
    start_time = millis();

    Serial.begin(115200);

    if (!run_wifi())
    {
        goSleepSec(5 * 60);
    }

    if (!run_dht())
    {
        goSleepSec(5 * 60);
    }

    if (!upload_tcp(readTemp(), readHum(), get_soilMois(), version.c_str(), "DHT11"))
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