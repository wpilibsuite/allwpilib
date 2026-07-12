# fields

The library where FIRST field images and their metadata are stored for use by other programs.

## Adding new field images

### Adding the image

Field images, if stored in PNG format, should be compressed with [oxipng](https://github.com/oxipng/oxipng) using `oxipng -o max --fast --strip safe -z fieldImage.png` to ensure the image is as small as possible. They should then be placed in `src/main/native/resources/org/wpilib/fields/<program>`, with the name `YEAR-gamename`, with `<program>` being either `ftc` or `frc`. For FTC, the year is a pair of years, like `2025-2026`.

### Adding the JSON

A JSON file with the same name should also be placed in the same location, with 6 fields:

- `game`, which should be the name of the game
- `field-image`, which contains the path of the field image relative to the directory containing the JSON file
- `field-corners`, an object that contains the two fields `top-left` and `bottom-right`, which are pairs of XY coordinates specifying the boundaries of the field in pixels
- `field-size`, which is a pair of lengths in the X and Y axes, respectively
- `field-unit`, the unit for `field-size` (always "foot")
- `program`, which is either `ftc` or `frc`

X is 0 at the left edge and increases to the right, and Y is 0 at the top edge and increases going down.

### Java updates

Add a new enum value to `src/main/java/org/wpilib/fields/Fields.java`. The enum value will be the path to the field JSON relative to the base resource directory stored in the enum.

### C++ updates

Create a new header in `src/main/native/include/wpi/fields/<progam>` called `YEAR-gamename.hpp`, and fill in this template with the year and game name.

```c++
// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

namespace wpi::fields {
std::string_view GetResource_<YEAR>_<gamename>_json();
std::string_view GetResource_<YEAR>_<gamename>_png();
}  // namespace wpi::fields
```

For FTC, `<YEAR>` should have the dash replaced with an underscore.

Finally, add the new field to the to the list of fields in `src/main/native/cpp/fields.cpp`, including the newly added header and using the functions inside.
