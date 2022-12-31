#include <SPI.h>
#include <LoRa.h>
#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial s(3, 4);

#define redPin A2
#define greenPin A1
#define bluePin A0
#define buzzPin 7

String inputString = "";      // String to hold input
String messageDeviceId = "";  // String to hold device ID
int val;
int initial = 1;
int flag1 = 0;
int flag2 = 0;
int flag3 = 0;
int alerted = 0;
String receiveData = "";
const char *del = ",";

// Function to setup the pins and LoRa module
void setup() {
  Serial.begin(9600);
  s.begin(9600);

  pinMode(redPin, OUTPUT);
  digitalWrite(redPin, LOW);
  pinMode(greenPin, OUTPUT);
  digitalWrite(greenPin, LOW);
  pinMode(bluePin, OUTPUT);
  digitalWrite(bluePin, LOW);
  pinMode(buzzPin, OUTPUT);
  digitalWrite(buzzPin, LOW);

  while (!Serial);
  Serial.println("LoRa Receiver");
  if (!LoRa.begin(433E6)) {  // or 915E6
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

// Function that will keep code execution through polling
void loop() {
  
  // Try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Read packet
    while (LoRa.available()) {
      inputString = LoRa.readString();
      receiveData += inputString;
    }
    inputString = "";
    LoRa.packetRssi();
  }

  int datalen = receiveData.length() + 1;
    String data1 = receiveData.substring(0, 1);
    String data2 = receiveData.substring(2, datalen);

    receiveData = "";

    if (data1 == "1" || initial == 1) {
      if (initial == 1 || flag1 == 0 || (flag1 == 1 && alerted == 0)) {
        initial = 0;
        Serial.println("Data Received 1 as Normal");
        s.print(String(data1) + "," + String(data2));
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, HIGH);
        digitalWrite(bluePin, LOW);
        digitalWrite(buzzPin, HIGH);
        delay(500);
        digitalWrite(buzzPin, LOW);
        if (flag1 == 0) {
          alerted = 1;
        }
        if (flag1 == 1 && alerted == 1) {
          alerted = 0;
        }
      }
      setFlagValues(1, 0, 0);
    } else if (data1 == "2") {
      if (flag2 == 0  || (flag2 == 1 && alerted == 0)) {
        Serial.println("Data Received 2 as Warning");
        s.print(String(data1) + "," + String(data2));
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
        if (flag2 == 0) {
          alerted = 1;
        }
        if (flag2 == 1 && alerted == 1) {
          alerted = 0;
        }
      }
      setFlagValues(0, 1, 0);
    } else if (data1 == "3") {
      if (flag3 == 0  || (flag3 == 1 && alerted == 0)) {
        Serial.println("Data Received 3 as Alert");
        s.print(String(data1) + "," + String(data2));
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
        if (flag3 == 0) {
          alerted = 1;
        }
        if (flag3 == 1 && alerted == 1) {
          alerted = 0;
        }
      }
      setFlagValues(0, 0, 1);
    }
}

// Function to set values for the flag variables
void setFlagValues(int flagNormal, int flagWarning, int flagAlert) {
  flag1 = flagNormal;
  flag2 = flagWarning;
  flag3 = flagAlert;
}