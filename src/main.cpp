#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <Wire.h>
#include "SHTSensor.h"
#include <Ticker.h>
#include <ArduinoJson.h>
#include <ESP8266httpUpdate.h>

const char *version = "1.7.3";

// ADC_MODE(ADC_VCC);

const char *host = "moe.swz1994.xyz";
const int httpsPort = 9527;

SHTSensor sht;
int sht_vcc = D5;

float temp = 0;
float hum = 0;

ulong start_time;

ESP8266WiFiMulti wifiMulti;

void goSleep(int sec)
{
  Serial.println("go sleep for " + String(sec));
  ESP.deepSleep(sec * 1000 * 1000);
}

void sht30_measure()
{
  if (sht.readSample())
  {
    hum = sht.getHumidity();
    temp = sht.getTemperature();
  }
  else
  {
    Serial.print("Error in readSample()\n");
  }
}

void update_started()
{
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished()
{
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total)
{
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err)
{
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void setup()
{
  start_time = millis();

  Serial.begin(115200);

  uint16 vcc = ESP.getVcc();
  Serial.println(vcc);
  if (vcc < 2800)
  {
    Serial.println("deep sleep max");
    ESP.deepSleep(ESP.deepSleepMax());
  }

  WiFi.mode(WIFI_STA);

  wifiMulti.addAP("HOME", "12345679");
  wifiMulti.addAP("HOME1", "12345679");
  wifiMulti.addAP("wanlaoshi", "jiejiemomo");
  wifiMulti.addAP("OnePlus7", "12345679");

  pinMode(sht_vcc, OUTPUT);
  digitalWrite(sht_vcc, HIGH);
  Wire.begin(D1, D2);

  if (sht.init())
  {
    Serial.print("sht init(): success\n");
  }
  else
  {
    Serial.print("sht init(): failed\n");
    goSleep(5 * 60);
  }

  sht.setAccuracy(SHTSensor::SHT_ACCURACY_HIGH);

  int wifiRetry = 0;
  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    wifiRetry++;
    if (wifiRetry > 60)
    {
      goSleep(5 * 60);
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  sht30_measure();

  WiFiClient client;

  if (!client.connect(host, httpsPort))
  {
    Serial.println("connection failed");
    goSleep(5 * 60);
  }

  // Serial.println(ESP.getVcc());

  const size_t capacity = JSON_OBJECT_SIZE(7);
  DynamicJsonDocument doc(capacity);

  int adc_reading;


  adc_reading = analogRead(A0);

  Serial.println(adc_reading);
  adc_reading = map(adc_reading, 0, 1023, 0, 1000);
  adc_reading = map(adc_reading, 0, 933, 0, 4200);
  Serial.println(adc_reading);

  doc["temp"] = temp;
  doc["hum"] = hum;
  doc["id"] = String(ESP.getChipId());
  doc["power"] = adc_reading;
  doc["version"] = version;
  doc["sensor_type"] = "SHT30";
  char buffer[160];

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
      goSleep(5 * 60);
    }
  }

  uint8 sleep = 5;
  uint8 sleep_read = client.read();
  if (sleep_read != -1 && sleep_read > 0)
  {
    sleep = sleep_read;
  }

  uint8 ota = client.read();

  Serial.println(sleep);
  Serial.println(ota);
  // Close the connection
  Serial.println();

  if (ota == 1)
  {
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    WiFiClientSecure otaClient;
    otaClient.setInsecure();

    t_httpUpdate_return ret = ESPhttpUpdate.update(otaClient, "https://moe.swz1994.xyz:1880/arduino/ota", version);
    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
    }

    Serial.println("closing connection");
  }

  client.stop();
  goSleep(sleep * 60);
}

void loop()
{
  if (millis() - start_time > 60 * 1000)
  {
    goSleep(5 * 60);
  }
  delay(1000);
}