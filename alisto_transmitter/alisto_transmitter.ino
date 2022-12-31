// Arduino Code for LoRa Transmitter
#include <SPI.h>
#include <LoRa.h>

#define lowPin 3
#define highPin 4

const int deviceId  = 1001;

int normalFloodState  = 1;
int warningFloodState  = 2;
int alertFloodState = 3;

int flag = 0;

// Function to setup the pins and LoRa module
void setup() {
  Serial.begin(9600);
  pinMode(lowPin, INPUT_PULLUP);
  pinMode(highPin, INPUT_PULLUP);

  while (!Serial);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(433E6)) {  // or 915E6, the MHz speed of your module
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

// Function that will keep code execution through polling
void loop() {
  if (digitalRead(lowPin) == LOW && digitalRead(highPin) == LOW) {
    // Flood State is Normal
    if (flag == 0) {
      dataSend(normalFloodState);
      Serial.println("Data Sent is 1 for Normal");
    }
    flag = 1;
  } else if (digitalRead(lowPin) == HIGH && digitalRead(highPin) == LOW) {
    // Flood State is Warning
    if (flag == 1) {
      dataSend(warningFloodState);
      Serial.println("Data Sent is 2 for Warning");
    }
    flag = 0;
  } else if (digitalRead(lowPin) == HIGH && digitalRead(highPin) == HIGH) {
    // Flood State is Alert
    if (flag == 0) {
      dataSend(alertFloodState);
      Serial.println("Data Sent is 3 for Alert");
    }
    flag = 1;
  }
}

void dataSend(int dataState) {
  String datasample = String(dataState) + "," + String(deviceId);
  LoRa.beginPacket();
  LoRa.print(datasample);
  LoRa.endPacket();
}