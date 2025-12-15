#pragma once

#include "wled.h"

// Usermod: White overlay (setzt alle LEDs weiß)
class WhiteOverlay : public Usermod {
  private:
    bool initDone = false;

  public:
    WhiteOverlay(const char* name, bool enabled) : Usermod(name, enabled) {}

    void setup() {
      // keine spezielle HW-Init nötig
      initDone = true;
    }

    // handleOverlayDraw() wird kurz vor show() aufgerufen -> wir können Farben setzen
    void handleOverlayDraw() {
      if (!initDone || !enabled) return;

      // Setze alle Pixel auf weiß (RGB voll). Bei RGBW-Strips bleibt White-Kanal 0.
      uint16_t len = strip.getLength();
      for (uint16_t i = 0; i < len; ++i) {
        strip.setPixelColor(i, RGBW32(255, 255, 255, 0));
      }
      // Kein strip.show() hier! handleOverlayDraw wird vor dem normalen show() aufgerufen.
    }

    // Optional: Status in /json/state anzeigen
    void addToJsonState(JsonObject &root) {
      if (!initDone) return;
      JsonObject u = root[FPSTR(_name)];
      if (u.isNull()) u = root.createNestedObject(FPSTR(_name));
      u["active"] = enabled;
    }

    // Optional: von UI/JSON steuerbar: {"WhiteOverlay":{"active":1}}
    void readFromJsonState(JsonObject &root) {
      JsonObject top = root[FPSTR(_name)];
      if (!top.isNull()) {
        int a = top["active"] | -1;
        if (a >= 0) enabled = (a != 0);
      }
    }

    uint16_t getId() { return USERMOD_ID_EXAMPLE; } // Unique ID (wie im Beispiel). Alternativ eigene ID definieren.
};