#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <Wire.h>
#include "SHTSensor.h"
#include <Ticker.h>
#include <ArduinoJson.h>
#include <ESP8266httpUpdate.h>

const char *version = "1.5";

ADC_MODE(ADC_VCC);

const char ssid[] = "wanlaoshi";
const char pass[] = "jiejiemomo";
const char *host = "moe.swz1994.xyz";
const int httpsPort = 9527;

IPAddress staticIP(192, 168, 2, 200);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(223,5,5,5);
IPAddress dns1(223,6,6,6);

SHTSensor sht;
int sht_vcc = D5;

float temp = 0;
float hum = 0;

ulong start_time;

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

  WiFi.config(staticIP, gateway, subnet, dns, dns1);
  WiFi.begin(ssid, pass);
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

  sht30_measure();

  int wifiRetry = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
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

  WiFiClient client;

  if (!client.connect(host, httpsPort))
  {
    Serial.println("connection failed");
    goSleep(5 * 60);
  }

  Serial.println(ESP.getVcc());

  const size_t capacity = JSON_OBJECT_SIZE(6);
  DynamicJsonDocument doc(capacity);

  doc["temp"] = temp;
  doc["hum"] = hum;
  doc["id"] = String(ESP.getChipId());
  doc["power"] = ESP.getVcc();
  doc["version"] = version;
  char buffer[121];

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
  if (sleep_read != -1) {
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
  if (millis() - start_time > 60*1000) {
    goSleep(5*60);
  }
  // timer1.update();
}