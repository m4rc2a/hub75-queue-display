# WLED Usermod: Scale_Text

**Automatically scale the text effect on your LED matrix to the largest possible size—using WLED’s grouping function per segment.**

`Scale_Text` is a WLED usermod that ensures your scrolling text or static messages always appear in the biggest, brightest way possible—no manual tuning needed!  
It leverages WLED’s native grouping feature to dynamically maximize font size, **per segment**. This means you can control which area of your matrix displays auto-scaled text, ideal for setups with multiple segments.

## Features

- Automatically scales text effect to fill each segment’s LED area
- Uses WLED’s grouping feature for optimal display on any matrix size
- Configurable via WLED’s web interface
- No manual font or grouping adjustments required
- Supports multiple segments with independent scaling

## Installation

1. **Copy files:**  
   Place `usermod_scale_text.h` in `wled00/usermods/` of your WLED-MM firmware source.

2. **PlatformIO Configuration:**  
   In your board’s section of `platformio.ini`, add:
   ```
   -D USERMOD_SCALE_TEXT
   ```

3. **Register the usermod in `usermods_list.cpp`:**

   At the top:

   ```cpp
   #ifdef USERMOD_SCALE_TEXT
   #include "../usermods/Scale_Text/usermod_scale_text.h"
   #endif
   ```

   Near the bottom (just before the final `}`):

   ```cpp
   #ifdef USERMOD_SCALE_TEXT
   usermods.add(new ScaleText("ScaleText", true));
   #endif
   ```

4. **Compile and upload your firmware** to the device.

## Configuration & Usage

### Segment-Based Scaling (Key Requirement!)

The auto-scaling function is **applied per segment**, since the grouping feature is segment-based in WLED.

**What you must do:**
1. **Create or select a segment:**  
   In the WLED web interface’s "Segments" menu, add or choose a segment where you want text to appear.

2. **Assign the "Text" effect to that segment:**  
   In the Effects tab, make sure the segment uses the "Text" effect. Only segments with this effect will be scaled by the usermod.

3. **Set the Segment ID in Scale_Text settings:**  
   Go to `Config > Usermods > Scale Text` and specify the Segment ID that should auto-scale.

4. **Enter your desired text:**  
   Type the text in the Effects/Text configuration box.  
   The usermod will automatically adjust grouping so the text fills as much space as possible within the selected segment.

---

### Example Usage

Let’s say you have a matrix split into two segments:
- Segment ID `0`: scrolling ticker for headlines
- Segment ID `1`: static display for a clock

You can auto-scale text independently by assigning the "Text" effect to each and entering those IDs in the usermod settings.

---

## How It Works

- The usermod detects the size of your chosen segment (width and height).
- It computes the optimal grouping value to maximize readable text.
- It sets WLED’s grouping parameter for that segment dynamically.

Changes are reflected immediately if you adjust segment size or switch text strings.

---

## Tips

- You can auto-scale text on multiple segments (with separate settings or custom builds).
- If you want to manually control grouping for some segments, leave those IDs unassigned in the usermod.
- Works with any matrix/layout supported by WLED/MM.

## Compatibility

- Adafruit MatrixPortal S3
- MoonModules/WLED-MM