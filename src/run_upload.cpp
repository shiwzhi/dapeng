#include "run_upload.h"
WiFiClient client;

const char *host = "dapeng.swz1994.xyz";
const int httpsPort = 9527;

uint8 sleep = 5;
bool ota = false;

bool upload_tcp(float temp, float hum, float soilTemp, const char *sensorType)
{
    if (!client.connect(host, httpsPort))
    {
        Serial.println("connection failed");
        return false;
    }

    const size_t capacity = JSON_OBJECT_SIZE(8);
    DynamicJsonDocument doc(capacity);

    float soilMois = analogRead(A0);
    Serial.println(soilMois);
    soilMois = map(soilMois, 0, 1023, 0, 1000);
    soilMois = map(soilMois, 180, 330, 100, 0);

    doc["temp"] = temp;
    doc["hum"] = hum;
    doc["id"] = String(ESP.getChipId());
    doc["soilMois"] = soilMois;
    doc["soilTemp"] = soilTemp;
    doc["sensor_type"] = sensorType;
    char buffer[200];

    serializeJson(doc, buffer);

    client.print(buffer);

    Serial.println("request sent");

    unsigned long timeout = millis();
    while (client.available() == 0)
    {
        if (millis() - timeout > 5000)
        {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return false;
        }
    }

    uint8 sleep_read = client.read();
    if (sleep_read != -1 && sleep_read > 0)
    {
        sleep = sleep_read;
    }

    client.stop();
    return true;
}

uint8 read_sleep()
{
    return sleep;
}