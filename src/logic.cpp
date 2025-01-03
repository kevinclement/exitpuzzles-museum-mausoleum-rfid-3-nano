#include "Arduino.h"
#include "logic.h"
#include "rfid.h"
#include "version.h"

Logic::Logic() : 
  rfid(*this)
{
}

void Logic::setup() {
  rfid.setup();
}

void Logic::handle() {
  rfid.handle();
}

void Logic::status() {
  char cMsg[254];
  sprintf(cMsg, 
    "status="
      "version:%s,"
      "gitDate:%s,"
      "buildDate:%s,"

      "idol_4:%s,"
      "idol_5:%s"

      "\r\n"
    , GIT_HASH,
      GIT_DATE,
      DATE_NOW,

      rfid.state[0] == CORRECT ? "true" : "false",
      rfid.state[1] == CORRECT ? "true" : "false"
  );

  Serial.print(cMsg);
}
