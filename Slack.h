#include <WiFiClientSecure.h>
#include "certs.h"
#include "SlackEnv.h"

X509List cert(cert_DigiCert_Global_Root_CA);

void createRequest(char *message) {
  WiFiClientSecure client;
  client.setTrustAnchors(&cert);
  if (!client.connect(slack_host, slack_port)) {
    return;
  }
  String data = String("{\"text\": \"") + message + "\"}";
  client.print(String("POST ") + slackPath + " HTTP/1.1\r\n" +
               "Host: " + slack_host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n" +
               "Content-Type: application/json; charset=utf-8\r\n" +
               "Content-Length: " + data.length() + "\r\n\r\n" +
                data + 
               "\r\n"
               );

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
    if (line == "\r") {
      Serial.println("Headers received");
      break;
    }
  }

  String line = client.readStringUntil('\n');
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}
