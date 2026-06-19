// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/DriverStationDisplay.hpp"

#include <cctype>
#include <chrono>
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

static constexpr std::string_view kClearDisplay = "\033[2J\033[H";
static wpi::util::mutex displayMutex;
static bool displayRawMode = false;
static wpi::util::StringMap<uint32_t> displayLineMap;
static std::vector<std::string> displayLines;
static std::string displayBuffer;

static void WriteAnsiText(std::string_view data) {
  WPI_String dataWpiStr = wpi::util::make_string(data);
  HAL_WriteDisplayAnsiText(&dataWpiStr);
}

void DriverStationDisplay::SetMode(Mode mode) {
  std::scoped_lock lock(displayMutex);

  switch (mode) {
    case Mode::Line:
      displayRawMode = false;
      displayLineMap.clear();
      displayLines.clear();
      break;
    case Mode::RawAnsi:
      displayRawMode = true;
      WriteAnsiText(kClearDisplay);
      break;
  }
}

void DriverStationDisplay::AddData(std::string_view caption,
                                   std::string_view line) {
  std::scoped_lock lock(displayMutex);
  if (displayRawMode) {
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
    displayLines.emplace_back(line);
    return;
  }

  uint32_t lineNum;
  auto it = displayLineMap.find(caption);
  if (it == displayLineMap.end()) {
    lineNum = displayLines.size();
    displayLineMap[caption] = lineNum;
    displayLines.emplace_back(line);
  } else {
    lineNum = it->second;
    if (lineNum < displayLines.size()) {
      displayLines[lineNum] = line;
    }
  }
}

void DriverStationDisplay::AddLine(std::string_view line) {
  AddData({}, line);
}

void DriverStationDisplay::UpdateLines() {
  std::scoped_lock lock(displayMutex);
  if (displayRawMode) {
    return;
  }

  static auto lastDisplayUpdate =
      wpi::hal::monotonic_clock::now() - std::chrono::milliseconds{230};
  auto now = wpi::hal::monotonic_clock::now();
  if (now - lastDisplayUpdate < std::chrono::milliseconds{230}) {
    displayLineMap.clear();
    displayLines.clear();
    return;
  }
  lastDisplayUpdate = now;

  displayBuffer.clear();
  displayBuffer += kClearDisplay;
  for (const auto& line : displayLines) {
    displayBuffer += line;
    displayBuffer += "\033[0m\n";
  }

  WPI_String wpiBuffer = wpi::util::make_string(displayBuffer);
  HAL_WriteDisplayAnsiText(&wpiBuffer);

  displayLineMap.clear();
  displayLines.clear();
}

void DriverStationDisplay::WriteRawAnsiText(std::string_view data) {
  std::scoped_lock lock(displayMutex);
  if (!displayRawMode) {
    return;
  }

  WriteAnsiText(data);
}

void DriverStationDisplay::ClearRaw() {
  std::scoped_lock lock(displayMutex);
  if (displayRawMode) {
    WriteAnsiText(kClearDisplay);
  }
}
