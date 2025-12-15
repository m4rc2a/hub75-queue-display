# WLED Usermod: External Serial Number Display

This WLED Usermod allows you to display numerical data received via UART directly on the LED matrix of your MatrixPortal S3 board. Ideal for displaying measurements, counter readings or other numerical values from external sensors or microcontrollers without having to operate the WLED interface.

Many ESP32 boards have several UART interfaces, including the MatrixPortalS3:

- UART0: Normally used for USB serial/debug (RX=3, TX=1)
- UART1: Available for user mods <-- This is used (Serial 1)
- UART2: Used by DMX if enabled SparkFunDMX.cpp:51

## Table of Contents

* [Features](#features)
* [Compatibility](#compatibility)
* [Installation](#installation)
  * [Place Files](#1-place-files)
  * [PlatformIO Configuration](#2-platformio-configuration)
  * [Compile and Upload](#3-compile-and-upload)
* [Configuration (WLED Web Interface)](#configuration-wled-web-interface)
* [Cabling](#cabling)
* [Usage](#usage)
* [Example (Binary Byte Transfer)](#example-binary-byte-transfer)

## Features

*   Receives serial data via configurable RX/TX pins (default: GPIO8 RX, GPIO18 TX).
*   Configurable baud rate.
*   Parses incoming data as integers.
*   Displays the received number as text on the LED matrix of the MatrixPortal S3.
*   Easy configuration via the WLED web interface.

## Compatibility

This usermod was developed specifically for the **Adafruit MatrixPortal S3** board in conjunction with the **MoonModules/WLED-MM** firmware, as it utilises the flexible GPIO assignment of the ESP32-S3 and the text display functions of WLED-MM.

## Installation

### 1. Place files

Place the following two files in the `wled00/usermods/` directory of your WLED-MM source code:

* `usermod_serial_int_display.h`

### 2. PlatformIO Configuration

In order for WLED to recognize and compile this usermod, you must enable it in the `platformio.ini` file of your WLED-MM project.

1.  Open the `platformio.ini` file (or `platformio_override.ini`, if you are using it).
2.  Find the `[env:...]` block that corresponds to your MatrixPortal S3 board (e.g., `[env:adafruit_matrixportal_s3]`).
2. Add the following line to the **beginning** of the file `usermods_list.cpp` in the `wled00` directory of this block:

    ```cpp
    #ifdef USERMOD_SERIAL_INT_DISPLAY
    #include "../usermods/Serial_int_Display/usermod_serial_int_display.h"
    #endif
    ```

3. Add the following line at the **end** of the file `usermods_list.cpp` **before** the `}` in the `wled00` directory of this block:

    ```cpp
    #ifdef USERMOD_SERIAL_INT_DISPLAY
    usermods.add(new SerialIntDisplay("SerialIntDisplay", true));
    #endif
    ```

4.  Add the following line to the `build_flags` of this block:

    ```ini
        -D USERMOD_SERIAL_INT_DISPLAY
    ```

    **Example for `platformio.ini`:**

    ```ini
    [env:adafruit_matrixportal_s3]
    board = adafruit_matrixportal_s3
    platform = espressif32@~6.0.1
    framework = arduino
    build_flags =
        ${common.build_flags}
        -D WLED_RELEASE_NAME=WLED-MM
        -D USERMOD_SERIAL_INT_DISPLAY  ; <-- Add this line!
        ; ... additional flags ...
    ```

### 3. Compile and upload

1.  **Save all changes** to the above files.
2.  **Compile the firmware** with PlatformIO (e.g., in VS Code via the checkmark icon at the bottom left or via the command line `pio run -e adafruit_matrixportal_s3`).
3.  **Upload the compiled firmware** to your MatrixPortal S3 board (e.g., via the right arrow icon in VS Code or `pio run -t upload -e adafruit_matrixportal_s3`).

## Configuration (WLED web interface)

Once the firmware has been successfully uploaded to your MatrixPortal S3 using the usermod:

1.  Open the WLED web interface in your browser.
2.  Under Effects, activate the **“Scrolling Text”** effect.
2.  Navigate to **“Config” -> “Usermods”**.
3.  There you should find an entry called **“Serial Int Display”**.
4.  Activate the user mod using the switch (if not already activated).
5.  You can now adjust the following settings:
    *   **RX Pin:** The GPIO pin that receives data (default: `8`).
    *   **TX Pin:** The GPIO pin that sends data (default: `18`).
    *   **Segment ID** The ID of the segment on which the text effect is located (default: `0`).
    *   **Baud Rate:** The transmission speed of the serial communication (default: `9600`). Must match the external device.
6.  Click **“Save”** and restart the board for the settings to take effect.

## Cabling

Connect your MatrixPortal S3 to the external serial device (e.g., another microcontroller or sensor) as follows:

| MatrixPortal S3 Pin | Function            | External Device Pin |
| :------------------ | :------------------ | :------------------ |
| **GPIO 8**          | RX (Receive)        | TX (Transmit)       |
| **GND**             | GND                 | GND                 |

**Important notes:**

*   **RX to TX, TX to RX:** Make sure that the transmit pin (TX) of one device is connected to the receive pin (RX) of the other device.
*   **Connect GND:** The ground pins (GND) of both devices must always be connected to each other to ensure a common reference voltage, otherwise you will only get garbage.
*   **Voltage level:** The GPIO pins of the ESP32-S3 operate at 3.3V. If your external serial device operates at 5V, you will need a [**logic level converter (level shifter)**](https://en.wikipedia.org/wiki/Comparator#Level_shifter) or you can build a [voltage divider](https://en.wikipedia.org/wiki/Voltage_divider) yourself between the devices to prevent damage to the MatrixPortal S3.

## Usage

In order to display a number on the MatrixPortal S3 display, it must be sent as an integer via the external serial interface.

## Example (Binary Byte Transfer)

If you want to display the number `42`, send it as a single byte (`uint8_t`).

### Sending from an external device (e.g., Arduino):

```cpp
uint8_t value = 42;
Serial.write(value); // Sends the single byte 0x2A (42 in decimal)
```

### Result on the LED matrix:

The number `42` is displayed on the LED matrix. It will remain displayed until a new value is received.

The user mod reads the received byte **directly as a binary value** (not as a character or string) and displays the corresponding integer on the matrix.  
If multiple bytes or unexpected data formats are received, the behaviour may not be as expected.

> **Note:**  
> The Arduino function `Serial.write(value)` sends a single byte (0–255), not a string or ASCII number.
