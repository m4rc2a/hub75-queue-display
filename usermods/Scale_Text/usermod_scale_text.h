#pragma once

#include "wled.h" // WLED Basis-Funktionen & Usermod-Basisklasse

#define PRINTABLE_CHAR_MIN 32     // Standard: Space
#define PRINTABLE_CHAR_MAX 127    // Standard: "~"
#define USERMOD_ID_SCALE_TEXT 201 // Einzigartige ID vergeben
#define SCALE_TEXT_MAX_FONT 5
#define SCALE_TEXT_MIN_FONT 1
#define SCALE_TEXT_MAX_GROUPING 10
#define SCALE_TEXT_MIN_GROUPING 1
#define SCALE_TEXT_DEFAULT_GAP 1 // Standard 1px, kann angepasst werden

class UsermodScaleText : public Usermod {
private:
  // --- Display/segment configuration ---
  int8_t segment_id; // WLED segment index to update
  int8_t default_scale;

  inline bool isPrintableCustom(char c) {
    return (c >= PRINTABLE_CHAR_MIN && c <= PRINTABLE_CHAR_MAX);
  }

public:
  UsermodScaleText(const char *name, bool enabled) : Usermod(name, enabled) {}

  bool textFitsInSegment(
      const Segment &seg, const char *text, int *outTextWidth = nullptr,
      int *outTextHeight = nullptr) // optionale Parameter für Debug
  {
    // Schriftgröße bestimmenk
    int fontSize = map(seg.custom2, 0, 255, 1, 5);
    int letterWidth, letterHeight;

    switch (fontSize) {
    default:
    case 1:
      letterWidth = 4;
      letterHeight = 6;
      break;
    case 2:
      letterWidth = 5;
      letterHeight = 8;
      break;
    case 3:
      letterWidth = 6;
      letterHeight = 8;
      break;
    case 4:
      letterWidth = 7;
      letterHeight = 9;
      break;
    case 5:
      letterWidth = 5;
      letterHeight = 12;
      break;
    }

    // Zeichen zählen (ASCII-Filter wie im Original)
    size_t count = 0;
    for (const char *p = text; *p; ++p)
      if (*p > 31 && *p < 128) // Zähle nur druckbare ASCII-Zeichen
        ++count;

    int physTextWidth = count * letterWidth * seg.grouping;
    int physTextHeight = letterHeight * seg.grouping;

    int segmentWidth = seg.width();
    int segmentHeight = seg.height();

    if (outTextWidth)
      *outTextWidth = physTextWidth;
    if (outTextHeight)
      *outTextHeight = physTextHeight;

    // Prüfen, ob der Text in das Segment passt
    return physTextWidth <= segmentWidth && physTextHeight <= segmentHeight;
  }

  /**
   * Skaliert die Schriftgröße (seg.custom2) und Gruppierung (seg.grouping)
   * so weit wie möglich – der Text muss noch ins Segment passen!
   * Gibt true zurück, wenn Anpassung erfolgreich.
   */
  /**
   * Skaliert Schriftgröße und Gruppierung, so dass seg->name möglichst groß
   * angezeigt wird und noch ins Segment passt. Ein Rand ("gap") kann
   * angegeben werden.
   *
   * @param seg     Das zu verarbeitende Segment
   * @param gap     Abstand (Pixel) zum Rand, z.B. 2. (Kann aus Settings kommen)
   *
   * Rückgabe: true, wenn Anpassung möglich, sonst false.
   */
  bool maximizeFontAndGrouping(Segment &seg,
                               uint8_t gap = SCALE_TEXT_DEFAULT_GAP) {
    const char *text = seg.name ? seg.name : "";

    bool fits = false;
    uint8_t font = SCALE_TEXT_MIN_FONT;
    uint8_t grouping = SCALE_TEXT_MIN_GROUPING;

    // Schritt 1: Maximale Font finden
    for (uint8_t f = SCALE_TEXT_MIN_FONT; f <= SCALE_TEXT_MAX_FONT; f++) {
      seg.custom2 = map(f, SCALE_TEXT_MIN_FONT, SCALE_TEXT_MAX_FONT, 0, 255);

      // Displayfläche um gap verkleinern
      int phys_x = seg.width() - 2 * gap;  // Links/rechts Rand beachten
      int phys_y = seg.height() - 2 * gap; // Oben/unten Rand beachten

      int dummyW, dummyH; // Falls gebraucht
      // Prüfe mit gepatchten Segmentmaßen (virtuelles Segment mit Abzug)
      Segment tmpSeg = seg;
      tmpSeg._widthRaw = max(1, phys_x);
      tmpSeg._heightRaw = max(1, phys_y);

      if (!textFitsInSegment(tmpSeg, text, &dummyW, &dummyH)) {
        font = (f > SCALE_TEXT_MIN_FONT) ? f - 1 : f;
        seg.custom2 =
            map(font, SCALE_TEXT_MIN_FONT, SCALE_TEXT_MAX_FONT, 0, 255);
        break;
      } else {
        font = f;
      }
    }

    // Schritt 2: Max grouping bestimmen
    for (uint8_t g = SCALE_TEXT_MIN_GROUPING; g <= SCALE_TEXT_MAX_GROUPING;
         g++) {
      seg.grouping = g;

      // Bereich wieder mit gap prüfen
      int phys_x = seg.width() - 2 * gap;
      int phys_y = seg.height() - 2 * gap;

      Segment tmpSeg = seg;
      tmpSeg._widthRaw = max(1, phys_x);
      tmpSeg._heightRaw = max(1, phys_y);

      if (!textFitsInSegment(tmpSeg, text)) {
        grouping = (g > SCALE_TEXT_MIN_GROUPING) ? g - 1 : g;
        seg.grouping = grouping;
        break;
      } else {
        grouping = g;
      }
    }
    // Anpassung erfolgreich, falls mind. minimal möglich
    return font >= SCALE_TEXT_MIN_FONT && grouping >= SCALE_TEXT_MIN_GROUPING;
  }

  void loop() override {
    if (!enabled || strip.isUpdating())
      return;

    // Hole das richtige Segment!
    if (segment_id < 0 || segment_id >= strip.getMaxSegments())
      return; // Schutz, falls ungültig

    Segment &seg = strip.getSegment(segment_id);

    // ---- Auto-Skalierung ----
    // Passe Font & Grouping optimal aufs Segment an!
    maximizeFontAndGrouping(seg, /* gap: */ 1);

// Jetzt kannst du sinnvoll weiterarbeiten!
// z.B. eigene Zeichnungsroutinen, Logging, Debug
// Optional: Status-Infos ausgeben
#ifdef SCALE_TEXT_DEBUG
    int tw, th;
    textFitsInSegment(seg, seg.name, &tw, &th);
    Serial.printf("AutoText: Font=%d, Grouping=%d, Text='%s', W=%d, H=%d\n",
                  map(seg.custom2, 0, 255, 1, 5), seg.grouping,
                  seg.name ? seg.name : "", tw, th);
#endif

    // Optional: strip.show() (meist handled WLED das)
  }

  /*
   * Adds persistent config to cfg.json.
   */
  void addToConfig(JsonObject &root) {
    Usermod::addToConfig(root);
    JsonObject top = root[FPSTR(_name)];
    top["segment_id"] = segment_id;
    top["default_scale"] = default_scale;
  }

  /*
   * Reads persistent config from cfg.json.
   * Returns true if all config values are present.
   */
  bool readFromConfig(JsonObject &root) {
    bool configComplete = Usermod::readFromConfig(root);
    JsonObject top = root[FPSTR(_name)];

    configComplete &= getJsonValue(top["segment_id"], segment_id, 0);
    configComplete &= getJsonValue(top["default_scale"], default_scale, 1);

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
  void appendConfigData() {
    // Add segment_id dropdown with all segments
    oappend(SET_F("dd2=addDropdown('"));
    oappend(String(FPSTR(_name)).c_str());
    oappend(SET_F("','segment_id');"));

    for (uint8_t i = 0; i < strip.getMaxSegments(); i++) {
      Segment *seg = &strip.getSegment(i);
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
    oappend(SET_F(":segment_id',1,'<i>Select the WLED segment to update (shows "
                  "index and name).</i>');"));
  }

  /*
   * Called before every show() (LED update frame) after effects.
   * Can be used to override LED colors (currently unused).
   */
  void handleOverlayDraw() {
    // Example: strip.setPixelColor(0, RGBW32(0,0,0,0));
  }

  uint16_t getId() override { return USERMOD_ID_SCALE_TEXT; }
};
