#pragma once

#include "wled.h"

class SerialIntDisplay : public Usermod {

private:
  // --- Serial configuration ---
  int8_t uart_rx_pin;    // UART RX pin number
  int8_t uart_tx_pin;    // UART TX pin number
  uint32_t baudrate;     // UART baudrate
  uint32_t prevBaudrate; // Stores last used baudrate for reinit

  // --- Display/segment configuration ---
  Segment *segment = nullptr; // pointer to WLED segment
  int8_t segment_id = 0;      // WLED segment index to update

  // --- State tracking ---
  uint8_t lastReceivedValue; // Last received value from serial
  bool firstReceived = true; // True until first serial value is received

public:
  /**
   * Constructor: Initializes usermod with name and enabled state.
   * No hardware is initialized here.
   */
  SerialIntDisplay(const char *name, bool enabled) : Usermod(name, enabled) {}

  void setSegmentPointer() {
    if (segment_id >= 0 && segment_id < strip.getMaxSegments()) {
      mySegment = &strip.getSegment(segment_id);
    } else {
      mySegment = nullptr;
    }
  }

  /**
   * Re-initialize Serial1 if baudrate has changed.
   */
  void reinitSerial() {
    if (initDone && baudrate != prevBaudrate) {
      Serial1.begin(baudrate, SERIAL_8N1, uart_rx_pin, uart_tx_pin);
      prevBaudrate = baudrate;
    }
  }

  /*
   * setup() is called once at boot. WiFi is not yet connected at this point.
   * Initializes Serial1 with configured pins and baudrate.
   */
  void setup() {
    Serial1.begin(baudrate, SERIAL_8N1, uart_rx_pin, uart_tx_pin);
    prevBaudrate = baudrate;
    initDone = true;
  }

  /*
   * loop() is called continuously.
   * Main logic:
   *  - Exit if usermod is disabled or strip is updating.
   *  - If serial data available, read one byte.
   *  - If value changed (or first received), update segment name and trigger
   * strip refresh.
   */
  void loop() {
    // Exit if usermod is disabled or LED strip is updating
    if (!enabled || strip.isUpdating())
      return;

    if (Serial1.available() >= 1) {
      uint8_t receivedByte = Serial1.read(); // Read a single byte (0-255)

      // Only update if value changed or first received
      if (firstReceived || lastReceivedValue != receivedByte) {
        lastReceivedValue = receivedByte;
        firstReceived = false;

        Segment *segment = &strip.getSegment(segment_id);

        if (segment && segment->name) {
          // TODO: Use WLED_MAX_SEGNAME_LEN for snprintf buffer size if
          // available
          //
          //       Pointer to Buffer
          //       |              Maximum bytes
          //       |              |   format magie
          //       |              |   |    Darzustellende Zahl
          //       |              |   |    |
          //       V              V   V    V
          snprintf(segment->name, 4, "%d",
                   lastReceivedValue); // e.g. 5 -> "5", 200 -> "200"
          strip.trigger();
        }
      }
    }
  }

  /*
   * Adds custom info to /json/info API.
   */
  void addToJsonInfo(JsonObject &root) {
    // Ensure "u" object exists
    JsonObject user = root["u"];
    if (user.isNull())
      user = root.createNestedObject("u");

    JsonArray dataArr = user.createNestedArray(FPSTR(_name));
    dataArr.add(lastReceivedValue);
    dataArr.add(F("last number"));
  }

  /*
   * Adds custom state to /json/state API (currently unused).
   */
  void addToJsonState(JsonObject &root) {
    if (!initDone || !enabled)
      return;
    JsonObject usermod = root[FPSTR(_name)];
    if (usermod.isNull())
      usermod = root.createNestedObject(FPSTR(_name));
  }

  /*
   * Reads custom state from /json/state API (currently unused).
   */
  void readFromJsonState(JsonObject &root) {
    if (!initDone)
      return;
    JsonObject usermod = root[FPSTR(_name)];
    if (!usermod.isNull()) {
      // TODO: Implement state update if needed
    }
  }

  /*
   * Adds persistent config to cfg.json.
   */
  void addToConfig(JsonObject &root) {
    Usermod::addToConfig(root);
    JsonObject top = root[FPSTR(_name)];
    top["uart_rx_pin"] = uart_rx_pin;
    top["uart_tx_pin"] = uart_tx_pin;
    top["segment_id"] = segment_id;
    top["baudrate"] = baudrate;
  }

  /*
   * Reads persistent config from cfg.json.
   * Returns true if all config values are present.
   */
  bool readFromConfig(JsonObject &root) {
    bool configComplete = Usermod::readFromConfig(root);
    JsonObject top = root[FPSTR(_name)];

    configComplete &= getJsonValue(top["uart_rx_pin"], uart_rx_pin, 8);
    configComplete &= getJsonValue(top["uart_tx_pin"], uart_tx_pin, 18);
    configComplete &= getJsonValue(top["segment_id"], segment_id, 0);
    configComplete &= getJsonValue(top["baudrate"], baudrate, 9600);

    // Prüfe, ob segment_id gültig ist (existiert)
    if (segment_id < 0 || segment_id >= strip.getMaxSegments()) {
      segment_id = 0; // Fallback auf 0, falls ungültig
      top["segment_id"] = segment_id;
    }

    reinitSerial();

    return configComplete;
  }

  /*
   * Appends additional config UI (baudrate dropdown) to usermod settings page.
   */
  void appendConfigData() {
    // Add info for UART RX pin
    oappend(SET_F("addInfo('"));
    oappend(String(FPSTR(_name)).c_str());
    oappend(SET_F(":uart_rx_pin',1,'<i>GPIO number for UART RX (receiving data "
                  "from external device).</i>');"));

    // Add info for UART TX pin
    oappend(SET_F("addInfo('"));
    oappend(String(FPSTR(_name)).c_str());
    oappend(SET_F(":uart_tx_pin',1,'<i>GPIO number for UART TX (transmitting "
                  "data to external device).</i>');"));

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

    // Add baudrate dropdown to settings page
    oappend(SET_F("dd=addDropdown('"));
    oappend(String(FPSTR(_name)).c_str());
    oappend(SET_F("','baudrate');"));

    oappend(SET_F("addOption(dd,'9600',9600);"));
    oappend(SET_F("addOption(dd,'19200',19200);"));
    oappend(SET_F("addOption(dd,'38400',38400);"));
    oappend(SET_F("addOption(dd,'57600',57600);"));
    oappend(SET_F("addOption(dd,'115200',115200);"));

    // Add info for baudrate
    oappend(SET_F("addInfo('"));
    oappend(String(FPSTR(_name)).c_str());
    oappend(SET_F(":baudrate',1,'<i>Select the baudrate for serial "
                  "communication (must match external device).</i>');"));
  }

  /*
   * Called before every show() (LED update frame) after effects.
   * Can be used to override LED colors (currently unused).
   */
  void handleOverlayDraw() {
    // Example: strip.setPixelColor(0, RGBW32(0,0,0,0));
  }

#ifndef WLED_DISABLE_MQTT
  /**
   * Handles MQTT messages (currently unused).
   */
  bool onMqttMessage(char *topic, char *payload) { return false; }

  /**
   * Called when MQTT connection is established (currently unused).
   */
  void onMqttConnect(bool sessionPresent) {
    // No MQTT initialization needed
  }
#endif

  /**
   * Called when WLED state changes (currently unused).
   */
  void onStateChange(uint8_t mode) {
    // No state change handling needed
  }

  /*
   * Returns unique usermod ID.
   */
  uint16_t getId() { return USERMOD_ID_SERIAL_INT_DISPLAY; }
};
