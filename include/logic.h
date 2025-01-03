#pragma once

#include "rfid.h"

class Logic {
public:
  Logic();
  Rfid rfid;

  void setup();
  void handle();
  void status();

private:
};

