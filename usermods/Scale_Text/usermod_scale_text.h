#pragma once

#include "wled.h" // WLED Basis-Funktionen & Usermod-Basisklasse

#define PRINTABLE_CHAR_MIN 31     // Standard: Space
#define PRINTABLE_CHAR_MAX 128    // Standard: "~"
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
  size_t charCount;

  // Hilfsmethode: Prüft, ob ein Zeichen druckbar ist
  inline bool isPrintableCustom(char c) {
    return (c >= PRINTABLE_CHAR_MIN && c <= PRINTABLE_CHAR_MAX);
  }

  // Hilfsmethode: Zählt druckbare Zeichen im Text
  inline size_t countChars(const char *text) {
    size_t count = 0;
    for (const char *p = text; *p; ++p)
      if (isPrintableCustom(*p))
        ++count;

    return count;
  }

  // Hilfsmethode: Berechnet Textbreite in Pixeln basierend auf Fontgröße und
  // Gruppierung
  int getTextPixelWidth(uint8_t fontSize, uint8_t grouping) {
    int letterWidth;

    switch (fontSize) {
    default:
    case 1: letterWidth = 4; break;
    case 2: letterWidth = 5; break;
    case 3: letterWidth = 6; break;
    case 4: letterWidth = 7; break;
    case 5: letterWidth = 5; break;
    }

    // TODO: spacing between letters?
    return charCount * letterWidth * grouping;
  }

  // Hilfsmethode: Berechnet Texthöhe in Pixeln basierend auf Fontgröße und
  // Gruppierung
  int getTextPixelHeight(uint8_t fontSize, uint8_t grouping) {
    int letterHeight;

    switch (fontSize) {
    default:
    case 1: letterHeight = 6; break;
    case 2: letterHeight = 8; break;
    case 3: letterHeight = 8; break;
    case 4: letterHeight = 9; break;
    case 5: letterHeight = 12; break;
    }

    return letterHeight * grouping;
  }

public:
  UsermodScaleText(const char *name, bool enabled) : Usermod(name, enabled) {}

  /**
   * Prüft, ob der Text mit den bestimmten Einstellungen in das Segment passt.
   *
   * @param segmentWidth   Breite des Segments in Pixeln
   * @param segmentHeight  Höhe des Segments in Pixeln
   * @param fontSize       Schriftgröße (1-5)
   * @param grouping       Gruppierung (1-10)
   * @param charCount      Anzahl der druckbaren Zeichen im Text
   *
   * @return true, wenn der Text passt, sonst false
   */
  bool textFitsInSegment(int segmentWidth, int segmentHeight, uint8_t fontSize,
                         uint8_t grouping, size_t charCount) {
    int physTextWidth = getTextPixelWidth(fontSize, grouping);
    int physTextHeight = getTextPixelHeight(fontSize, grouping);

    return physTextWidth <= segmentWidth && physTextHeight <= segmentHeight;
  }

  /**
   * Skaliert Schriftgröße und Gruppierung, so dass seg->name möglichst groß
   * angezeigt wird und noch ins Segment passt. Dabei wird auch die möglichst
   * größte Font gewählt. Ein Rand ("gap") kann angegeben werden.
   *
   * @param seg     Das zu verarbeitende Segment
   * @param gap     Abstand (Pixel) zum Rand, z.B. 2. (Kann aus Settings
   * kommen)
   *
   * @return true, wenn Anpassung erfolgreich, sonst false.
   */
  bool maximizeFontAndGrouping(Segment &seg,
                               uint8_t gap = SCALE_TEXT_DEFAULT_GAP) {
    const char *text = seg.name ? seg.name : "";

    bool fits = false;
    uint8_t font = SCALE_TEXT_MIN_FONT;
    uint8_t grouping = SCALE_TEXT_MIN_GROUPING;

    int textSpaceX = seg.width() - 2 * gap;  // Links/rechts Rand beachten
    int textSpaceY = seg.height() - 2 * gap; // Oben/unten Rand beachten

    // Maximale Font finden
    for (uint8_t f = SCALE_TEXT_MIN_FONT; f <= SCALE_TEXT_MAX_FONT; f++) {
      // Displayfläche um gap verkleinern

      if (textFitsInSegment(textSpaceX, textSpaceY, f, grouping, charCount)) {
        if (f >= SCALE_TEXT_MAX_FONT) {
          font = f;
          break;
        }
      } else {
        if (f >= SCALE_TEXT_MIN_FONT) {
          font = SCALE_TEXT_MIN_FONT;
        } else {
          font = f - 1;
        }
        break;
      }
    }

    // Max grouping bestimmen
    for (uint8_t g = SCALE_TEXT_MIN_GROUPING; g <= SCALE_TEXT_MAX_GROUPING;
         g++) {

      if (textFitsInSegment(textSpaceX, textSpaceY, font, g, charCount)) {
        if (g >= SCALE_TEXT_MAX_GROUPING) {
          grouping = g;
          break;
        }
      } else {
        if (g >= SCALE_TEXT_MIN_GROUPING) {
          grouping = SCALE_TEXT_MIN_GROUPING;
        } else {
          grouping = g - 1;
        }
        break;
      }
    }

    seg.custom1 = font;     // Schriftgröße setzen
    seg.custom2 = grouping; // Gruppierung setzen

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

    charCount = countChars(seg.name ? seg.name : ""); // TODO: nur bei Änderung

    // ---- Auto-Skalierung ---- TODO
    // Passe Font & Grouping optimal aufs Segment an!
    maximizeFontAndGrouping(seg, /* gap: */ 1);

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
