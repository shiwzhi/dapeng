#include <Arduino.h>
#include "run_wifi.h"
#include "run_dht.h"
#include "run_ota.h"
#include "run_upload.h"
#include "run_ds18b20.h"

String version = "2.0.5";
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
    int soil_vcc = 4;
    pinMode(soil_vcc, OUTPUT);
    digitalWrite(soil_vcc, HIGH);
    setup_dht();
    start_time = millis();

    Serial.begin(115200);

    if (!run_wifi())
    {
        goSleepSec(5 * 60);
    }

    run_ota(version);

    if (!run_dht())
    {
        goSleepSec(5 * 60);
    }

    if (!run_ds18b20()) {
        goSleepSec(5 * 60);
    }

    if (!upload_tcp(readTemp(), readHum(), get_soilTemp(), "DHT11"))
    {
        goSleepSec(5 * 60);
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