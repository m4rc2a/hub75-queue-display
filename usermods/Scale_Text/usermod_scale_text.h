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

    /*
     * Adds persistent config to cfg.json.
     */
    void addToConfig(JsonObject& root)
    {
      Usermod::addToConfig(root);
      JsonObject top = root[FPSTR(_name)];
      top["segment_id"] = segment_id;
    }

    /*
     * Reads persistent config from cfg.json.
     * Returns true if all config values are present.
     */
    bool readFromConfig(JsonObject& root)
    {
      bool configComplete = Usermod::readFromConfig(root);
      JsonObject top = root[FPSTR(_name)];

      configComplete &= getJsonValue(top["segment_id"], segment_id, 0);

      // Prüfe, ob segment_id gültig ist (existiert)
      if (segment_id < 0 || segment_id >= strip.getMaxSegments()) {
        segment_id = 0; // Fallback auf 0, falls ungültig
        top["segment_id"] = segment_id;
      }

      return configComplete;
    }

    /*
     * Appends additional config UI (baudrate dropdown) to usermod settings page.
     */
    void appendConfigData()
    {
      // Add segment_id dropdown with all segments
      oappend(SET_F("dd2=addDropdown('"));
      oappend(String(FPSTR(_name)).c_str());
      oappend(SET_F("','segment_id');"));

      for (uint8_t i = 0; i < strip.getMaxSegments(); i++) {
        Segment* seg = &strip.getSegment(i);
        char segLabel[40];

        if (seg && seg->name && seg->name[0] != '\0') {
          snprintf(segLabel, sizeof(segLabel), "%u: %s", i, seg->name);
        } else {
          snprintf(segLabel, sizeof(segLabel), "%u", i);
        }

        oappend(SET_F("addOption(dd2,'"));
        oappend(segLabel);
        oappend(SET_F("',"));
        oappend(String(i).c_str());
        oappend(SET_F(");"));
      }

      oappend(SET_F("addInfo('"));
      oappend(String(FPSTR(_name)).c_str());
      oappend(SET_F(":segment_id',1,'<i>Select the WLED segment to update (shows index and name).</i>');"));
    }

    /*
     * Called before every show() (LED update frame) after effects.
     * Can be used to override LED colors (currently unused).
     */
    void handleOverlayDraw()
    {
      // Example: strip.setPixelColor(0, RGBW32(0,0,0,0));
    }

    uint16_t getId() override {
      return USERMOD_ID_SCALE_TEXT;
    }
};
