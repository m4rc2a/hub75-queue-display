# Ticket Number Display System with WLED-MM

**This firmware is a customized fork of [MoonModules/WLED-MM](https://github.com/MoonModules/WLED) for ticket and queue number display.**

Upstream updates from WLED-MM are regularly merged.

All “_Ticket System_” features are maintained in separate usermods and build config for easy merging.

## Project Overview

- **Purpose:** Realize a ticket/queue system where numbers are drawn on a large LED matrix.
- **Workflow:**  
  1. A number is drawn and printed using a thermal printer controlled by an Arduino.
  2. After printing, the Arduino sends the number via UART to the MatrixPortal S3 running WLED-MM.
  3. The number is displayed on the HUB75 LED matrix.

## Hardware Support

- **LED Matrix Controller:**
  - Adafruit MatrixPortal S3
  - **Other Boards:** Other ESP32 boards may work, but only MatrixPortal S3 is tested.

- **LED Matrix:** 
  - HUB75 compatible RGB panel

- **Communication:**
  - UART (default RX: GPIO8, TX: GPIO18)

## Features

- Receives numbers via UART and displays them on the LED matrix.
- Simple integration with external ticket/queue systems (e.g., Arduino-based).
- Web UI for configuration and manual testing (WLED-MM).
- All standard WLED-MM features are available.

## Usermods Used in This Project

WLED “usermods” are modular code extensions designed to add new features without directly editing the core WLED firmware. This makes it easier to maintain compatibility, update to newer WLED versions, and share your work with other projects.

For this ticket display system, included two custom usermods:

- [**Serial_int_Display**](usermods/Serial_int_Display/readme.md)
  Receives a byte value via the serial port (UART) and displays it using the WLED text effect.  
  This lets external microcontrollers (like Arduinos) easily send numbers for display.

- [**Scale_Text**](usermods/Scale_Text/readme.md)
  Automatically scales text to fit the LED matrix’s resolution for maximum readability.
  This makes text effects robust for different display sizes and is reusable for other WLED setups.

## Quick Start

1. **Flash the firmware** in this repo to your MatrixPortal S3 (see [Installation](#installation)).
2. **Connect UART** from your Arduino (TX) to MatrixPortal S3 (RX, GPIO8), and GND to GND.
3. **Send numbers** as one Byte Numbers from the Arduino after printing.
4. The number will be displayed on the matrix

## Installation

1. Clone this repo.
2. Install dependencies and build the web UI:
   ```bash
   npm ci
   npm run build
   ```
3. Build and upload the firmware for MatrixPortal S3:
   ```bash
   pio run -e adafruit_matrixportal_s3
   pio run -e adafruit_matrixportal_s3 --target upload
   ```
4. Configure the Serial-to-Display usermod via the WLED web UI under **Config > Usermods**.

See [`usermods/Serial_int_Display/readme.md`](usermods/Serial_int_Display/readme.md) for detailed wiring and configuration instructions.

## Repository Structure

```
wled00/                 # Main firmware source (C++)
  ├── data/            # Web interface files 
  ├── usermods/        # Usermod source (Serial_to_Display)
  └── ...              # Other WLED-MM sources
tools/                 # Build tools (Node.js)
platformio.ini         # Hardware build configuration
package.json           # Node.js dependencies and scripts
```

## TODOs

- [ ] Implement auto-scaling for number size based on screen resolution. (**WIP**)
- [ ] Set up Continuous Integration (CI) for automated testing, builds, and deployments (**WIP**)
- [ ] Releases and provide downloadable builds for easy access (**WIP**)
- [ ] add nix-direnv for faster Development setup
- [ ] Implement automated testing to ensure code quality and stability
- [ ] Create a custom font with larger characters for better readability and higher resolution support.
- [ ] Animated transitions when new numbers are received and displayed.
- [ ] Further improvements to usability and visual appearance.
- [ ] Add a QR code to the display for connecting to the Wi-Fi.
- [ ] Rename the `Serial_int_Display` usermod to a more generic and descriptive name, since it currently receives a byte (not an int). The new name should reflect its present and future capabilities.

**WIP**: Work in Progress

## Special thanks to  
- [WLED](https://github.com/Aircoookie/WLED)  
- [WLED-MM (MoonModules)](https://github.com/MoonModules/WLED)  
- [Siemens Professional Education (SPE)](https://www.siemens.com/de/de/unternehmen/jobs/siemens-professional-education.html)

## License 
The WLED-MM and the **hub75-queue-display** projects are licensed under the [EUPL-1.2](https://joinup.ec.europa.eu/collection/eupl) or later.
The official license text is [available in 23 languages](https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12).
