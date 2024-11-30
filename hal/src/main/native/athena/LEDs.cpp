// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/LEDs.h"

#include <unistd.h>

#include <cstring>
#include <fstream>

#include <fmt/format.h>
#include <fmt/std.h>
#include <wpi/fs.h>

#include "HALInternal.h"
#include "hal/Errors.h"

namespace hal::init {

void InitializeLEDs() {
  int32_t status = 0;
  HAL_SetRadioLEDState(HAL_RadioLED_kOff, &status);
}
}  // namespace hal::init

static const fs::path radioLEDGreenFilePath =
    "/sys/class/leds/nilrt:wifi:primary/brightness";
static const fs::path radioLEDRedFilePath =
    "/sys/class/leds/nilrt:wifi:secondary/brightness";

static const char* onStr = "1";
static const char* offStr = "0";

static bool ReadStateFromFile(fs::path path, int32_t* status) {
  std::error_code ec;
  fs::file_t file = fs::OpenFileForRead(path, ec, fs::OF_Text);
  if (ec) {
    hal::SetLastError(status, fmt::format("Could not open '{}' for read: {}",
                                          path, ec.message()));
    *status = INCOMPATIBLE_STATE;
    return false;
  }
  // We only need to read one byte because the file won't have leading zeros.
  char buf[1]{};
  ssize_t count = read(file, buf, 1);
  // save errno, always close file.
  int err = errno;
  fs::CloseFile(file);
  if (count <= 0) {
    *status = INCOMPATIBLE_STATE;
    if (count == 0) {
      hal::SetLastError(status,
                        fmt::format("Read from '{}' returned no data.", path));
    } else {
      hal::SetLastError(status, fmt::format("Failed to read from '{}': {}",
                                            path, std::strerror(err)));
    }
    return false;
  }
  // If the brightness is not zero, the LED is on.
  return buf[0] != '0';
}

extern "C" {
void HAL_SetRadioLEDState(HAL_RadioLEDState state, int32_t* status) {
  std::error_code ec;
  fs::file_t greenFile = fs::OpenFileForWrite(radioLEDGreenFilePath, ec,
                                              fs::CD_OpenExisting, fs::OF_Text);
  if (ec) {
    // not opened, nothing to clean up
    *status = INCOMPATIBLE_STATE;
    hal::SetLastError(status, fmt::format("Could not open '{}' for write: {}",
                                          greenFile, ec.message()));
    return;
  }
  fs::file_t redFile = fs::OpenFileForWrite(radioLEDRedFilePath, ec,
                                            fs::CD_OpenExisting, fs::OF_Text);
  if (ec) {
    // green file opened successfully, need to close it
    fs::CloseFile(greenFile);
    *status = INCOMPATIBLE_STATE;
    hal::SetLastError(status, fmt::format("Could not open '{}' for write: {}",
                                          greenFile, ec.message()));
    return;
  }

  write(greenFile, state & HAL_RadioLED_kGreen ? onStr : offStr, 1);
  write(redFile, state & HAL_RadioLED_kRed ? onStr : offStr, 1);

  fs::CloseFile(greenFile);
  fs::CloseFile(redFile);
}

HAL_RadioLEDState HAL_GetRadioLEDState(int32_t* status) {
  bool green = ReadStateFromFile(radioLEDGreenFilePath, status);
  bool red = ReadStateFromFile(radioLEDRedFilePath, status);
  if (*status == 0) {
    if (green && red) {
      return HAL_RadioLED_kOrange;
    } else if (green) {
      return HAL_RadioLED_kGreen;
    } else if (red) {
      return HAL_RadioLED_kRed;
    } else {
      return HAL_RadioLED_kOff;
    }
  } else {
    return HAL_RadioLED_kOff;
  }
}
}  // extern "C"
