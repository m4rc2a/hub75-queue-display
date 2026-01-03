# WLED Usermod: Scale_Text

**Automatically select the optimal font size and scaling (“grouping”) for your text on WLED LED matrices, so your message is as big as possible**

## What does this usermod currently do?

- Calculates the largest possible font size that can fit your given text in the provided segment (usually your LED matrix).
- Uses WLED’s "grouping" feature to additionally scale text until it fills the available segment area.
- The logic is implemented in the `maximizeFontAndGrouping` function, with additional helper methods for sizing and printable-character detection.

**Note:**
- The usermod currently just maximizes font size and grouping *based on the text and segment size.  
- No optimizations for efficiency are implemented yet—this calculation happens each loop.

## Status / Current Limitations

- Triggering of `maximizeFontAndGrouping`: Right now, this is called every cycle in the main loop —-> no intelligent change detection yet.
- Configuration is minimal and direct. The main value: always have clear, large text filling your segment.

## Future Plans

- [ ] Efficient updating  
  - Only call `maximizeFontAndGrouping()` when the text changes, not on every loop. Improves performance.

- [ ] Use WLED JSON API for segment updates  
  - Switch from direct memory writes to official WLED API for better compatibility and reliability.

- [x] Configurable gap/margin support  <!-- Already implemented! -->
  - *Needs testing*
    - Allow users to specify a minimum gap between text and segment edges, preventing visual cutoffs.
    - [ ] Should be configurable via the usermod settings UI (not just code/static value).

- [ ] Translate all code comments, variable/function names, and documentation from German to English

## Example Usage

1. **Assign a “Text” effect segment in the WLED web interface.**
2. **This usermod will automatically maximize font size and grouping** so that your text fills the matrix as much as possible.

## ⚙️ Installation

1. Copy `usermod_scale_text.h` to `wled00/usermods/` in your WLED source tree.
2. Add to your `platformio.ini` (e.g. under your build environment):
   -D USERMOD_SCALE_TEXT
   
4. Register the usermod in `usermods_list.cpp`:
```cpp
#ifdef USERMOD_SCALE_TEXT
#include "../usermods/Scale_Text/usermod_scale_text.h"
usermods.add(new UsermodScaleText("Scale_Text", true));
#endif
```

Compile and upload.


## Compatibility

Developed for WLED-MM and similar forks (ESP32, MatrixPortal S3, etc.)
Should work wherever WLED’s segment and text/grouping features exist.
