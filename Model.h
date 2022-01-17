#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "Slack.h"

const uint8_t segmentSeconds = 450;
const uint8_t periodSeconds = 90;

uint8_t currentTimeIndex = -1;

bool periods[5];
uint8_t timeSegments[192];

uint8_t currentMonth, currentDay;

void postToSlack(uint8_t month, uint8_t day, uint32_t sumSeconds) {
  uint8_t hour = sumSeconds / 60 / 60;
  uint8_t minutes = sumSeconds / 60 % 60;
  float workingRatio = sumSeconds / 86400.0 * 100.0;

  char message[60];
  sprintf(message, "%d月%d日の稼働時間数: %d時間%d分（%0.1f%%）", month, day, hour, minutes, workingRatio);

  createRequest(message);
}

void process(uint8_t month, uint8_t day, bool isSensorHigh, uint32_t currentSeconds) {
  uint8_t timeIndex = currentSeconds / segmentSeconds;  // 0-192

  if (currentTimeIndex != timeIndex) {
    if (currentTimeIndex != -1) {
      uint8_t sum = 0;
      for (uint8_t i = 0; i < 5; i++) {
        sum += periods[i] ? 1 : 0;
      }

      timeSegments[currentTimeIndex] = sum;
    }

    if (timeIndex == 0) {
      uint32_t sum = 0;
      for (uint8_t i = 0; i < 192; i++) {
        sum += timeSegments[i] * periodSeconds;
      }
      postToSlack(currentMonth, currentDay, sum);

      for (uint8_t i = 0; i < 192; i++) {
        timeSegments[i] = 0;
      }
    }

    for (uint8_t i = 0; i < 5; i++) {
      periods[i] = 0;
    }

    currentTimeIndex = timeIndex;
    currentMonth = month;
    currentDay = day;
  }

  uint8_t periodIndex = currentSeconds % segmentSeconds / periodSeconds;
  if (!periods[periodIndex]) {
    periods[periodIndex] = isSensorHigh;
  }
}
