#include "run_upload.h"
WiFiClient client;

const char *host = "dapeng.swz1994.xyz";
const int httpsPort = 9527;

uint8 sleep = 5;
bool ota = false;

bool upload_tcp(float temp, float hum, const char *version, const char *sensorType)
{
    if (!client.connect(host, httpsPort))
    {
        Serial.println("connection failed");
        return false;
    }

    const size_t capacity = JSON_OBJECT_SIZE(7);
    DynamicJsonDocument doc(capacity);

    doc["temp"] = temp;
    doc["hum"] = hum;
    doc["id"] = String(ESP.getChipId());
    doc["power"] = ESP.getVcc();
    doc["version"] = version;
    doc["sensor_type"] = sensorType;
    char buffer[155];

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

    if (client.read() == 1)
    {
        ota = true;
    }

    client.stop();
    return true;
}

uint8 read_sleep()
{
    return sleep;
}

bool read_ota()
{
    return ota;
}