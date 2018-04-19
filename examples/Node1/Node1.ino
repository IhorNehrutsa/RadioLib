#include "KiteLib.h"

#define LED_START_ERROR   A1
#define LED_START_OK      A0
#define LED_RECEIVING     A3
#define LED_TRANSMITING   A2

#define BLINK_DELAY       250

XBee bee = Kite.ModuleA;
RF69 rf = Kite.ModuleB;

Packet pack;

void setup() {
  Serial.begin(9600);

  pinMode(LED_START_ERROR, OUTPUT);
  pinMode(LED_START_OK, OUTPUT);
  pinMode(LED_RECEIVING, OUTPUT);
  pinMode(LED_TRANSMITING, OUTPUT);

  ledsHigh();

  Serial.print(F("[XBee] Initializing ... "));
  byte state = bee.begin(9600);
  if(state == ERR_NONE) {
    Serial.println(F("success!"));
    ledsLow();
    delay(BLINK_DELAY);
    ledsHigh();
  } else {
    Serial.print(F("failed, code 0x"));
    Serial.println(state, HEX);
    ledsLow();
    digitalWrite(LED_START_ERROR, HIGH);
    while(true);
  }

  Serial.print(F("[XBee] Setting PAN ID ... "));
  state = bee.setPanId("0123456789ABCDEF");
  if(state == ERR_NONE) {
    Serial.println(F("success!"));
    ledsLow();
    delay(BLINK_DELAY);
    ledsHigh();
  } else {
    Serial.print(F("failed, code 0x"));
    Serial.println(state, HEX);
    ledsLow();
    digitalWrite(LED_START_ERROR, HIGH);
    while(true);
  }

  Serial.print(F("[XBee] Setting destination address ... "));
  state = bee.setDestinationAddress("0013A200", "40A58A5D");
  if(state == ERR_NONE) {
    Serial.println(F("success!"));
    ledsLow();
    delay(BLINK_DELAY);
    ledsHigh();
  } else {
    Serial.print(F("failed, code 0x"));
    Serial.println(state, HEX);
    ledsLow();
    digitalWrite(LED_START_ERROR, HIGH);
    while(true);
  }

  Serial.print(F("[RF69] Initializing ... "));
  state = rf.begin();
  if(state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code 0x"));
    Serial.println(state, HEX);
    ledsLow();
    digitalWrite(LED_START_ERROR, HIGH);
    while(true);
  }

  ledsLow();
  digitalWrite(LED_START_OK, HIGH);
}

void loop() {
  Serial.print(F("[RF69] Waiting for incoming transmission ... "));
  byte state = rf.receive(pack);

  if(state == ERR_NONE) {
    digitalWrite(LED_RECEIVING, HIGH);
    Serial.println(F("success!"));

    char str[24];

    pack.getSourceStr(str);
    Serial.print(F("[RF69] Source:\t\t"));
    Serial.println(str);

    pack.getDestinationStr(str);
    Serial.print(F("[RF69] Destination:\t"));
    Serial.println(str);

    Serial.print(F("[RF69] Length:\t\t"));
    Serial.println(pack.length);

    Serial.print(F("[RF69] Data:\t\t"));
    Serial.println(pack.data);

    digitalWrite(LED_RECEIVING, LOW);
    
    Serial.print(F("[XBee] Sending packet ... "));
    digitalWrite(LED_TRANSMITING, HIGH);
    bee.println(pack.data);
    digitalWrite(LED_TRANSMITING, LOW);
    Serial.println(F("done!"));
    
  } else if(state == ERR_RX_TIMEOUT) {
    Serial.println(F("timeout!"));
    
  } else if(state == ERR_CRC_MISMATCH) {
    Serial.println(F("CRC error!"));
    
  }
}

void ledsHigh() {
  digitalWrite(LED_START_ERROR, HIGH);
  digitalWrite(LED_START_OK, HIGH);
  digitalWrite(LED_RECEIVING, HIGH);
  digitalWrite(LED_TRANSMITING, HIGH);
}

void ledsLow() {
  digitalWrite(LED_START_ERROR, LOW);
  digitalWrite(LED_START_OK, LOW);
  digitalWrite(LED_RECEIVING, LOW);
  digitalWrite(LED_TRANSMITING, LOW);
}

