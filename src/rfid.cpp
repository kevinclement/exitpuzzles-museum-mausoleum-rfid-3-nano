#include "Arduino.h"
#include "rfid.h"
#include "logic.h"
#include <SPI.h>

#define OFFSET 4

MFRC522 mfrc522_1(8, 9);
MFRC522 mfrc522_2(10, 9);


byte tags [2][2][4] = {
  {
    { 0x04, 0xE9, 0x21, 0x0A },
    { 0x87, 0x93, 0x8E, 0xF2 }
  },
  {
    { 0x27, 0x8F, 0x8E, 0xF2 },
    { 0xF7, 0x92, 0x8E, 0xF2 }
  }
};

Rfid::Rfid(Logic &logic)
: _logic(logic)
{
}

void Rfid::setup() {
  SPI.begin();

  mfrc522_1.PCD_Init();
  mfrc522_2.PCD_Init();

  delay(4);
  mfrc522_1.PCD_DumpVersionToSerial();
  mfrc522_2.PCD_DumpVersionToSerial();

  // store addresses
  mfr[0] = &mfrc522_1;
  mfr[1] = &mfrc522_2;

  Serial.println("\nReady to Scan...");
}

void Rfid::handle() {
  for (uint8_t i = 0; i < NR_OF_READERS; i++) {
    RFID_STATE st = checkForTag(i, mfr[i]);
    if (st != state[i]) {
      Serial.print("state changed for ");
      Serial.print(i + OFFSET);
      Serial.print(" ");
      Serial.print(prettyState(state[i]));
      Serial.print(" => ");
      Serial.println(prettyState(st));

      state[i] = st;
      _logic.status();
    }
  }
}

RFID_STATE Rfid::checkForTag(uint8_t index, MFRC522 *mfr) {
  RFID_STATE st = state[index];
  tag_present_prev[index] = tag_present[index];

  error_counter[index] += 1;
  if(error_counter[index] > 2){
    tag_found[index] = false;
  }

  // Detect Tag without looking for collisions
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);

  // Reset baud rates
  mfr->PCD_WriteRegister(mfr->TxModeReg, 0x00);
  mfr->PCD_WriteRegister(mfr->RxModeReg, 0x00);

  // Reset ModWidthReg
  mfr->PCD_WriteRegister(mfr->ModWidthReg, 0x26);

  MFRC522::StatusCode result = mfr->PICC_RequestA(bufferATQA, &bufferSize);

  if(result == mfr->STATUS_OK){
    if ( ! mfr->PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue   
      return st;
    }
    error_counter[index] = 0;
    tag_found[index] = true;

    for ( uint8_t i = 0; i < 4; i++) {
       readCards[index][i] = mfr->uid.uidByte[i];
    }
  }

  tag_present[index] = tag_found[index];

  // rising edge
  if (tag_present[index] && !tag_present_prev[index]){
    Serial.println("Tag found, checking...");
    st = compareTags(index) ? CORRECT : INCORRECT;
  }

  // falling edge
  if (!tag_present[index] && tag_present_prev[index]){
    Serial.println("Tag gone");
    st = MISSING;
  }

  return st;
}

bool Rfid::compareTags(uint8_t index) {
  for ( uint8_t i = 0; i < 2; i++ ) {
    bool cardMatch = true;
    for ( uint8_t j = 0; j < 4; j++ ) {
        cardMatch = cardMatch && (readCards[index][j] == tags[index][i][j]);
    }

    if (cardMatch) {
      return true;
    }
  }

  return false;
}

String Rfid::prettyState(uint8_t state) {
  return 
    state == INCORRECT ? "Incorrect" : 
    state == CORRECT ? "Correct" : 
    state == MISSING ? "Missing" : 
    "Unknown";
}