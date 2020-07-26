#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>


bool upload_tcp(float temp, float hum, const char* sensorType);

uint8 read_sleep(); 
