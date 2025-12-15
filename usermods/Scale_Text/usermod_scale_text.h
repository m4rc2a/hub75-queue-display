#pragma once

#include "wled.h"  // WLED Basis-Funktionen & Usermod-Basisklasse

#define USERMOD_ID_SCALE_TEXT 201  // Einzigartige ID vergeben

class UsermodScaleText : public Usermod {
  private:
    // --- Display/segment configuration ---
    int8_t segment_id;       // WLED segment index to update

  public:
    UsermodScaleText(const char* name, bool enabled) : Usermod(name, enabled) {}

    void setup() override {
      // Initialisierungscode
    }

    void loop() override {
      // Hauptlogik, wird ständig ausgeführt
    }

    uint16_t getId() override {
      return USERMOD_ID_SCALE_TEXT;
    }
};
