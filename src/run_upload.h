#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>


bool upload_tcp(float temp, float hum, float soilTemp, const char* sensorType);

uint8 read_sleep(); 

