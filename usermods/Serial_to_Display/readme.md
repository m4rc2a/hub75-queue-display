# WLED Usermod: External Serial Number Display

Diese WLED Usermod ermöglicht es dir, numerische Daten, die über eine externe serielle Schnittstelle (UART) empfangen werden, direkt auf der LED-Matrix deines MatrixPortal S3 Boards anzuzeigen. Ideal, um Messwerte, Zählerstände oder andere Zahlenwerte von externen Sensoren oder Mikrocontrollern sichtbar zu machen, ohne die WLED-Oberfläche bedienen zu müssen.

Auf ESP32 gibt es mehrere UART-Schnittstellen:

- UART0: Wird normalerweise für USB-Serial/Debug verwendet (RX=3, TX=1) wled.h:321-329
- UART1: Verfügbar für Usermods <-- Diese wird verwendet (Serial 1)
- UART2: Wird von DMX verwendet, falls aktiviert SparkFunDMX.cpp:51


## Inhaltsverzeichnis

*   [Features](#features)
*   [Kompatibilität](#kompatibilität)
*   [Installation](#installation)
    *   [Dateien platzieren](#dateien-platzieren)
    *   [Platfo< rmIO Konfiguration](#platformio-konfiguration)
    *   [Kompilieren und Hochladen](#kompilieren-und-hochladen)
*   [Konfiguration (WLED Web-Interface)](#konfiguration-wled-web-interface)
*   [Verkabelung](#verkabelung)
*   [Nutzung](#nutzung)
*   [Beispiel Usermod Code](#beispiel-usermod-code)

## Features

*   Empfängt serielle Daten über konfigurierbare RX/TX-Pins (Standard: GPIO8 RX, GPIO18 TX).
*   Konfigurierbare Baudrate.
*   Parst eingehende Daten als Ganzzahlen.
*   Zeigt die empfangene Zahl als Text auf der LED-Matrix des MatrixPortal S3 an.
*   Einfache Konfiguration über das WLED Web-Interface.

## Kompatibilität

Dieser Usermod wurde speziell für das **Adafruit MatrixPortal S3** Board in Verbindung mit der **MoonModules/WLED-MM** Firmware entwickelt, da er die flexible GPIO-Zuweisung des ESP32-S3 und die Textanzeigefunktionen von WLED-MM nutzt.

## Installation

### 1. Dateien platzieren

Lege die folgenden zwei Dateien im Verzeichnis `wled00/usermods/` deines WLED-MM Quellcodes ab:

*   `usermod_serial_display.h`

### 2. PlatformIO Konfiguration

Damit WLED deinen Usermod erkennt und kompiliert, musst du ihn in der `platformio.ini` Datei deines WLED-MM Projekts aktivieren.

1.  Öffne die Datei `platformio.ini` (oder `platformio_override.ini`, falls du diese verwendest).
2.  Suche den `[env:...]` Block, der deinem MatrixPortal S3 Board entspricht (z.B. `[env:adafruit_matrixportal_s3]`).
3.  Füge die folgende Zeile zu den `build_flags` dieses Blocks hinzu:

    ```ini
    build_flags =
        ${common.build_flags}
        -D USERMOD_SERIAL_DISPLAY ; <-- Füge diese Zeile hinzu!
        ; ... weitere Flags ...
    ```

    **Beispiel für `platformio.ini`:**

    ```ini
    [env:adafruit_matrixportal_s3]
    board = adafruit_matrixportal_s3
    platform = espressif32@~6.0.1
    framework = arduino
    build_flags =
        ${common.build_flags}
        -D WLED_RELEASE_NAME=WLED-MM
        -D USERMOD_SERIAL_DISPLAY
        ; ... andere WLED_DISABLE-Flags oder spezifische Flags für dein Board ...
    ```

### 3. Kompilieren und Hochladen

1.  **Speichere alle Änderungen** an den oben genannten Dateien.
2.  **Kompiliere die Firmware** mit PlatformIO (z.B. in VS Code über das Häkchen-Symbol unten links oder über die Befehlszeile `pio run -e adafruit_matrixportal_s3`).
3.  **Lade die kompilierte Firmware** auf dein MatrixPortal S3 Board hoch (z.B. über den Pfeil-nach-rechts-Symbol in VS Code oder `pio run -t upload -e adafruit_matrixportal_s3`).

## Konfiguration (WLED Web-Interface)

Nachdem die Firmware mit dem Usermod erfolgreich auf dein MatrixPortal S3 hochgeladen wurde:

1.  Öffne das WLED Web-Interface in deinem Browser.
2.  Navigiere zu **"Config" -> "Usermods"**.
3.  Dort solltest du einen Eintrag **"External Serial Number Display"** finden.
4.  Aktiviere den Usermod über den Schalter.
5.  Du kannst nun die folgenden Einstellungen anpassen:
    *   **RX Pin:** Der GPIO-Pin, der Daten empfängt (Standard: `8`). <!-- TODO -->
    *   **TX Pin:** Der GPIO-Pin, der Daten sendet (Standard: `18`).   
    *   **Baud Rate:** Die Übertragungsgeschwindigkeit der seriellen Kommunikation (Standard: `9600`). Muss mit dem externen Gerät übereinstimmen.
6.  Klicke auf **"Save"** und starte das Board neu, damit die Einstellungen wirksam werden.

## Verkabelung

Verbinde dein MatrixPortal S3 mit dem externen seriellen Gerät (z.B. einem anderen Mikrocontroller oder Sensor) wie folgt:

| MatrixPortal S3 Pin | Funktion            | Externes Gerät Pin | Funktion            |
| :------------------ | :------------------ | :----------------- | :------------------ |
| **GPIO 8**          | RX (Empfangen)      | TX (Senden)        | TX (Senden)         |
| **GPIO 18**         | TX (Senden)         | RX (Empfangen)     | RX (Empfangen)      |
| **GND**             | Masse               | GND                | Masse               |

**Wichtige Hinweise:**

*   **RX an TX, TX an RX:** Achte darauf, dass der Sende-Pin (TX) des einen Geräts mit dem Empfangs-Pin (RX) des anderen Geräts verbunden ist und umgekehrt.
*   **GND verbinden:** Die Massepins (GND) beider Geräte müssen immer miteinander verbunden sein, um eine gemeinsame Referenzspannung zu gewährleisten sonst kommt nur Kauderwelsch an.
*   **Spannungspegel:** Die GPIO-Pins des ESP32-S3 arbeiten mit 3.3V. Wenn dein externes serielles Gerät mit 5V arbeitet, benötigst du einen **Logikpegelwandler (Level Shifter)** zwischen den Geräten, um Schäden am MatrixPortal S3 zu vermeiden.

## Nutzung

Um eine Zahl auf dem MatrixPortal S3 Display anzuzeigen, sende die Zahl als Text gefolgt von einem Zeilenumbruch (`\n`) über die externe serielle Schnittstelle.

### Tip:

Verwende `Serial.println("1337")` und nicht `Serial.print("1337\n")`

**Beispiel:**

Wenn du die Zahl `42` anzeigen möchtest, sende die Zeichenfolge `"42\n"`.

*   **Senden vom externen Gerät (z.B. Arduino):**
    ```cpp
    Serial.println(42); // Sendet "42" gefolgt von einem Zeilenumbruch
    ```

*   **Ergebnis auf dem MatrixPortal S3:**
    Die Zahl "42" wird für die konfigurierte Dauer (Standard: 5 Sekunden) auf der LED-Matrix angezeigt. Nach Ablauf der Zeit wird der Text gelöscht, bis eine neue Zahl empfangen wird.

Der Usermod parst die empfangene Zeichenkette und versucht, sie in eine Ganzzahl umzuwandeln. Nicht-numerische Zeichenketten oder ungültige Zahlen werden ignoriert.