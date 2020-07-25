#include "run_wifi.h"

ESP8266WiFiMulti wifiMulti;

void setup_wifi()
{
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP("wlw", "wxp123456");
    wifiMulti.addAP("HOME", "12345679");
    wifiMulti.addAP("HOME1", "12345679");
    wifiMulti.addAP("wanlaoshi", "jiejiemomo");
    wifiMulti.addAP("OnePlus7", "12345679");
}

bool run_wifi()
{
    setup_wifi();
    int wifiRetry = 0;
    while (wifiMulti.run() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        wifiRetry++;
        if (wifiRetry > 60)
        {
            Serial.println("wifi connected failed");
            return false;
        }
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    return true;
}