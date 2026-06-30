// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/DriverStationDisplay.hpp"

#include <cctype>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/hal/DriverStation.h"
#include "wpi/hal/monotonic_clock.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/string.hpp"

using namespace wpi;

static constexpr std::chrono::milliseconds UPDATE_PERIOD{230};

static constexpr std::string_view CLEAR_DISPLAY = "\033[0m\033[2J\033[H";

namespace {
struct DriverStationDisplayStorage {
  wpi::util::mutex mutex;
  bool rawMode = false;
  wpi::util::StringMap<uint32_t> lineMap;
  std::vector<std::string> lines;
  std::string buffer;
};
}  // namespace

static DriverStationDisplayStorage& GetDisplayStorage() {
  static DriverStationDisplayStorage storage;
  return storage;
}

static void WriteDisplayAnsiToHal(std::string_view data) {
  WPI_String dataWpiStr = wpi::util::make_string(data);
  HAL_WriteDisplayAnsi(&dataWpiStr);
}

void DriverStationDisplay::SetMode(Mode mode) {
  auto& storage = GetDisplayStorage();
  std::scoped_lock lock(storage.mutex);

  switch (mode) {
    case Mode::Line:
      storage.rawMode = false;
      storage.lineMap.clear();
      storage.lines.clear();
      break;
    case Mode::RawAnsi:
      storage.rawMode = true;
      WriteDisplayAnsiToHal(CLEAR_DISPLAY);
      break;
  }
}

// @Common - This is one of the commonly used methods for this class
void DriverStationDisplay::AddData(std::string_view caption,
                                   std::string_view line) {
  auto& storage = GetDisplayStorage();
  std::scoped_lock lock(storage.mutex);
  if (storage.rawMode) {
    return;
  }

  bool hasCaption = false;
  for (char ch : caption) {
    if (!std::isspace(static_cast<unsigned char>(ch))) {
      hasCaption = true;
      break;
    }
  }
  if (!hasCaption) {
    storage.lines.emplace_back(line);
    return;
  }

  uint32_t lineNum;
  auto it = storage.lineMap.find(caption);
  if (it == storage.lineMap.end()) {
    lineNum = storage.lines.size();
    storage.lineMap[caption] = lineNum;
    storage.lines.emplace_back(line);
  } else {
    lineNum = it->second;
    if (lineNum < storage.lines.size()) {
      storage.lines[lineNum] = line;
    }
  }
}

// @Common - This is one of the commonly used methods for this class
void DriverStationDisplay::AddLine(std::string_view line) {
  AddData({}, line);
}

void DriverStationDisplay::UpdateLines() {
  auto& storage = GetDisplayStorage();
  std::scoped_lock lock(storage.mutex);
  if (storage.rawMode) {
    return;
  }

  static auto lastDisplayUpdate =
      wpi::hal::monotonic_clock::now() - UPDATE_PERIOD;
  auto now = wpi::hal::monotonic_clock::now();
  if (now - lastDisplayUpdate < UPDATE_PERIOD) {
    storage.lineMap.clear();
    storage.lines.clear();
    return;
  }
  lastDisplayUpdate = now;

  storage.buffer.clear();
  storage.buffer += CLEAR_DISPLAY;
  for (const auto& line : storage.lines) {
    storage.buffer += line;
    storage.buffer += "\033[0m\n";
  }

  WPI_String wpiBuffer = wpi::util::make_string(storage.buffer);
  HAL_WriteDisplayAnsi(&wpiBuffer);

  storage.lineMap.clear();
  storage.lines.clear();
}

void DriverStationDisplay::WriteRawAnsi(std::string_view data) {
  auto& storage = GetDisplayStorage();
  std::scoped_lock lock(storage.mutex);
  if (!storage.rawMode) {
    return;
  }

  WriteDisplayAnsiToHal(data);
}

void DriverStationDisplay::ClearRaw() {
  auto& storage = GetDisplayStorage();
  std::scoped_lock lock(storage.mutex);
  if (storage.rawMode) {
    WriteDisplayAnsiToHal(CLEAR_DISPLAY);
  }
}
