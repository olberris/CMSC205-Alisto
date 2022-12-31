#include <time.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "HTTPSRedirect.h"

ESP8266WiFiMulti WiFiMulti;
SoftwareSerial myConnection(D4, D3);

// Variable use
const char *ssid = "Galaxy Z Fold43A61";      // wifi name
const char *password = "mxyb5292";  // wifi password

const char *host = "script.google.com";
const char *GScriptId = "AKfycbxxBx2xBE0tB-_c1qbyrz3M-AofEbq8Bo3Oq0bTrAqxSzRrupaSsuouTSzJkQqzjR_C";
const int httpsPort = 443;

#define redPin D8
#define greenPin D7
#define bluePin D6
#define buzzPin D5

int id = 1;
String byteBuffer = "";
String inputString = "";  // String to hold input
String messageDeviceId = "";
String receiveData = "";

HTTPSRedirect *client = nullptr;

int waterlevel;

void setup() {
  Serial.begin(115200);
  myConnection.begin(9600);

  for (uint8_t t = 4; t > 0; t--) {
    Serial.flush();
    delay(1000);
  }

  pinMode(redPin, OUTPUT);
  digitalWrite(redPin, LOW);
  pinMode(greenPin, OUTPUT);
  digitalWrite(greenPin, LOW);
  pinMode(bluePin, OUTPUT);
  digitalWrite(bluePin, LOW);
  pinMode(buzzPin, OUTPUT);
  digitalWrite(buzzPin, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to wifi.");
  digitalWrite(buzzPin, HIGH);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    digitalWrite(buzzPin, LOW);
    delay(250);
    digitalWrite(buzzPin, HIGH);
    Serial.print(".");
  }
  digitalWrite(buzzPin, HIGH);
  Serial.println("Wi-Fi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Connecting to ");
  Serial.println(host);
}

void loop() {
  processDataAndBlinkLED();
}

void processDataAndBlinkLED() {
  digitalWrite(buzzPin, HIGH);
  if (myConnection.available() > 0) {
    inputString = myConnection.readString();
    receiveData += inputString;
  }
  inputString = "";

  int datalen = receiveData.length() + 1;
  String data1 = receiveData.substring(0, 1);
  String data2 = receiveData.substring(2, datalen);

  if (String(data1) == "1") {
    Serial.println("Data Receive 1");
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);
    digitalWrite(buzzPin, HIGH);
    delay(500);
    digitalWrite(buzzPin, LOW);
    sendDataToGoogleSheets(data1, String(data2));
  } else if (String(data1) == "2") {
    Serial.println("Data Receive 2");
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, HIGH);
    digitalWrite(buzzPin, HIGH);
    delay(500);
    digitalWrite(buzzPin, LOW);
    delay(150);
    digitalWrite(buzzPin, HIGH);
    delay(500);
    digitalWrite(buzzPin, LOW);
    sendDataToGoogleSheets(data1, String(data2)); 
  } else if (String(data1) == "3") {
    Serial.println("Data Receive 3");
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
    digitalWrite(buzzPin, HIGH);
    delay(2000);
    digitalWrite(buzzPin, LOW);
    delay(200);
    digitalWrite(buzzPin, HIGH);
    delay(100);
    digitalWrite(buzzPin, LOW);
    delay(200);
    digitalWrite(buzzPin, HIGH);
    delay(100);
    digitalWrite(buzzPin, LOW);
    sendDataToGoogleSheets(data1, String(data2));
  }
  receiveData = "";
}

void sendDataToGoogleSheets(String waterLevel, String deviceId) {
  static int error_count = 0;
  static int connect_count = 0;
  const unsigned int MAX_CONNECT = 20;

  String wlevel = "";
  if (waterLevel == "1") {
    wlevel = "Normal";
  } else if (waterLevel == "2") {
    wlevel = "Warning";
  } else if (waterLevel == "3") {
    wlevel = "Alert";
  }

  //Serial.println("DeviceId = " + deviceId);

  // Write to Google Spreadsheet
  String url = String("/macros/s/") + GScriptId + "/exec?deviceid=" + deviceId + "&warninglevel=" + wlevel;

  static bool flag = false;

  if (!flag) {
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }

  if (client != nullptr) {
    if (!client->connected()) {
      client->connect(host, httpsPort);
    }
  } else {
    Serial.println("Error creating client object!");
    error_count = 5;
  }

  if (connect_count > MAX_CONNECT) {
    connect_count = 0;
    flag = false;
    delete client;
    return;
  }

  if (client->GET(url, host)) {
    ++connect_count;
  } else {
    ++error_count;
    Serial.println("Error-count while connecting: ");
    Serial.println(error_count);
  }

  if (error_count > 3) {
    Serial.println("Halting device...");
    delete client;
    client = nullptr;
    Serial.flush();
    ESP.deepSleep(0);
  }

  id++;
  delay(4000);
}