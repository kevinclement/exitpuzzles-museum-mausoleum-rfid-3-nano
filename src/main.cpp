#include <Arduino.h>
#include "logic.h"

Logic logic;

void(* resetFunc) (void) = 0;

void readAnySerialMessage() {
  if (!Serial.available()) {
    return;
  }

  // read and handle message from serial
  String msg = Serial.readStringUntil('\n');
  Serial.print("got '");
  Serial.print(msg);
  Serial.println("' command");

  if (msg == "reset" || msg == "reboot" || msg == "r") {
    resetFunc();
  } else {
    Serial.print("unknown command: ");
    Serial.println(msg);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10);
  Serial.println("READY");
  Serial.println("RFID Reader for rfid 4 and 5 (NANO) by kevinc...\n");

  logic.setup();
  logic.status();
}

void loop() {
  readAnySerialMessage();
  logic.handle();
}
