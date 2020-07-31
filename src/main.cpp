#include <Arduino.h>
#include "run_wifi.h"
#include "run_ota.h"
#include "run_upload.h"
#include "run_sht30.h"
#include "run_ds18b20.h"

ADC_MODE(ADC_VCC);
const char* version = "2.0.2";
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
    start_time = millis();
    Serial.begin(115200);

    if (!run_wifi())
    {
        goSleepSec(5 * 60);
    }

    run_ota(version);

    if (!run_sht())
    {
        goSleepSec(5 * 60);
    }

    if (!run_ds18b20()) {
        goSleepSec(5 * 60);
    }

    if (!upload_tcp(readTemp(), readHum(), "SHT30"))
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