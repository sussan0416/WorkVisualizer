#include "Model.h"
#include "Env.h"
#include <time.h>

#define TIMEZONE_JST  "JST-9"  // Arduino/cores/esp8266/TZ.h
#define NTP_SERVER1   "ntp.nict.jp"  // NTPサーバー

int retryCount = 0;

void setup() {
  Serial.begin(74880);
  setupInput();
  ntpSync();
}

void loop() {
  time_t now;
  struct tm *timeptr;

  now = time(nullptr);
  timeptr = localtime(&now);
  uint32_t secondsOfDay = timeptr->tm_sec + timeptr->tm_min * 60 + timeptr->tm_hour * 3600;
  
  process(timeptr->tm_mon+1, timeptr->tm_mday, digitalRead(5), secondsOfDay);
  
  delay(1000);
}

void setupInput() {
  pinMode(5, INPUT);
}

void ntpSync() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retryCount++;

    if (retryCount > 600) {
      delay(1000);
      ESP.reset();
    }
  }
  retryCount = 0;

  configTzTime(TIMEZONE_JST, NTP_SERVER1);
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
}
