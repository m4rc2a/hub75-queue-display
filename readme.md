# Ticket Number Display System with WLED-MM

This repository contains a customized [MoonModules/WLED-MM](https://github.com/MoonModules/WLED) firmware, tailored for use as a ticket or queue number display system. The system is designed for the **Adafruit MatrixPortal S3** with a HUB75 RGB LED matrix, and features a usermod for displaying numbers received via UART.

## Project Overview

- **Purpose:** Realize a ticket/queue system where numbers are drawn, printed, and displayed on a large LED matrix.
- **Workflow:**  
  1. A number is drawn and printed using a thermal printer controlled by an Arduino.
  2. After printing, the Arduino sends the number via UART to the MatrixPortal S3 running WLED-MM.
  3. The number is displayed prominently on the HUB75 LED matrix.
- **Firmware:** This repo contains only the customized WLED-MM firmware with the Serial-to-Display usermod enabled.

## Hardware

- **Display Controller:** Adafruit MatrixPortal S3
- **LED Matrix:** HUB75 compatible RGB panel
- **Communication:** UART (default RX: GPIO8, TX: GPIO18)
- **Other Boards:** Other ESP32-S3 boards may work, but only MatrixPortal S3 is tested.

## Features

- Receives numbers via UART and displays them on the LED matrix.
- Simple integration with external ticket/queue systems (e.g., Arduino-based).
- Web UI for configuration and manual testing (WLED-MM).
- All standard WLED-MM features are available.

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

See [`usermods/Serial_to_Display/readme.md`](usermods/Serial_to_Display/readme.md) for detailed wiring and configuration instructions.

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

## Future Plans

- [ ] Integration of a larger and more readable font for number display.
- [ ] Animated transitions when new numbers are received and displayed.
- [ ] Further improvements to usability and visual appearance.

## Special Thanks

Special thanks to  
- [WLED](https://github.com/Aircoookie/WLED)  
- [WLED-MM (MoonModules)](https://github.com/MoonModules/WLED)  
- Siemens Professional Education (SPE)

## License 
WLED-MM is licensed under the [EUPL-1.2](https://joinup.ec.europa.eu/collection/eupl) or later.  
The official license text is [available in 23 languages](https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12).