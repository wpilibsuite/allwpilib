// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/GamepadSelectable.hpp"

#include <atomic>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/driverstation/DriverStationDisplay.hpp"
#include "wpi/driverstation/Gamepad.hpp"

using namespace wpi;

static std::atomic_int instances;

static std::string FormatDouble(double value) {
  std::ostringstream stream;
  stream << std::fixed << std::setprecision(12) << value;
  std::string result = stream.str();
  while (result.size() > 2 && result.back() == '0' &&
         result[result.size() - 2] != '.') {
    result.pop_back();
  }
  return result;
}

GamepadSelectable::Chooser::Chooser(std::string_view name,
                                    std::vector<std::string> options)
    : m_name{name}, m_options{std::move(options)} {}

std::string_view GamepadSelectable::Chooser::GetName() const {
  return m_name;
}

const std::vector<std::string>& GamepadSelectable::Chooser::GetOptions() const {
  return m_options;
}

std::string_view GamepadSelectable::Chooser::GetSelected() const {
  return m_options[m_selectedIndex];
}

int GamepadSelectable::Chooser::GetSelectedIndex() const {
  return m_selectedIndex;
}

void GamepadSelectable::Chooser::SelectPrevious() {
  m_selectedIndex = Wrap(m_selectedIndex - 1, static_cast<int>(m_options.size()));
}

void GamepadSelectable::Chooser::SelectNext() {
  m_selectedIndex = Wrap(m_selectedIndex + 1, static_cast<int>(m_options.size()));
}

GamepadSelectable::GamepadSelectable(int port)
    : GamepadSelectable{DriverStation::GetGamepad(port)} {}

GamepadSelectable::GamepadSelectable(Gamepad& gamepad)
    : m_gamepad{&gamepad},
      m_captionPrefix{"GamepadSelectable/" + std::to_string(instances++) +
                      "/"} {}

Gamepad& GamepadSelectable::GetGamepad() {
  return *m_gamepad;
}

const Gamepad& GamepadSelectable::GetGamepad() const {
  return *m_gamepad;
}

GamepadSelectable::Chooser& GamepadSelectable::AddOptions(
    std::string_view name, std::vector<std::string> options) {
  RequireChooserName(name);
  if (options.empty()) {
    throw std::invalid_argument("Options cannot be empty");
  }

  std::string nameString{name};
  if (m_chooserMap.contains(nameString)) {
    throw std::invalid_argument("Chooser already exists: " + nameString);
  }

  auto chooser =
      std::unique_ptr<Chooser>{new Chooser{nameString, std::move(options)}};
  auto& chooserRef = *chooser;
  m_chooserMap.emplace(std::move(nameString), chooser.get());
  m_choosers.emplace_back(std::move(chooser));
  return chooserRef;
}

GamepadSelectable::Chooser& GamepadSelectable::AddOptions(
    std::string_view name, std::initializer_list<std::string_view> options) {
  std::vector<std::string> optionStrings;
  optionStrings.reserve(options.size());
  for (auto option : options) {
    optionStrings.emplace_back(option);
  }
  return AddOptions(name, std::move(optionStrings));
}

GamepadSelectable::Chooser& GamepadSelectable::AddIntegerOptions(
    std::string_view name, int min, int max, int delta) {
  if (min > max) {
    throw std::invalid_argument("Minimum cannot be greater than maximum");
  }
  if (delta <= 0) {
    throw std::invalid_argument("Delta must be greater than zero");
  }

  std::vector<std::string> options;
  for (int64_t value = min; value <= max; value += delta) {
    options.emplace_back(std::to_string(value));
  }
  if (options.back() != std::to_string(max)) {
    options.emplace_back(std::to_string(max));
  }
  return AddOptions(name, std::move(options));
}

GamepadSelectable::Chooser& GamepadSelectable::AddDoubleOptions(
    std::string_view name, double min, double max, double delta) {
  if (!std::isfinite(min) || !std::isfinite(max) || !std::isfinite(delta)) {
    throw std::invalid_argument("Minimum, maximum, and delta must be finite");
  }
  if (min > max) {
    throw std::invalid_argument("Minimum cannot be greater than maximum");
  }
  if (delta <= 0.0) {
    throw std::invalid_argument("Delta must be greater than zero");
  }

  std::vector<std::string> options;
  double previous = -std::numeric_limits<double>::infinity();
  for (int step = 0;; step++) {
    double value = min + delta * step;
    if (value > max) {
      break;
    }
    if (value <= previous) {
      throw std::invalid_argument("Delta is too small to produce another option");
    }
    options.emplace_back(FormatDouble(value));
    previous = value;
  }
  std::string maximumString = FormatDouble(max);
  if (options.back() != maximumString) {
    options.emplace_back(std::move(maximumString));
  }
  return AddOptions(name, std::move(options));
}

void GamepadSelectable::Update() {
  if (m_choosers.empty()) {
    return;
  }

  if (m_gamepad->GetDpadUpButtonPressed()) {
    m_selectedChooser =
        Wrap(m_selectedChooser - 1, static_cast<int>(m_choosers.size()));
  }
  if (m_gamepad->GetDpadDownButtonPressed()) {
    m_selectedChooser =
        Wrap(m_selectedChooser + 1, static_cast<int>(m_choosers.size()));
  }

  Chooser& chooser = *m_choosers[m_selectedChooser];
  if (m_gamepad->GetDpadLeftButtonPressed()) {
    chooser.SelectPrevious();
  }
  if (m_gamepad->GetDpadRightButtonPressed()) {
    chooser.SelectNext();
  }

  for (size_t i = 0; i < m_choosers.size(); i++) {
    const Chooser& displayChooser = *m_choosers[i];
    DriverStationDisplay::AddData(
        m_captionPrefix + std::string{displayChooser.GetName()},
        FormatDisplayLine(displayChooser,
                          i == static_cast<size_t>(m_selectedChooser)));
  }
}

std::string_view GamepadSelectable::GetSelected(std::string_view name) const {
  return GetChooser(name)->GetSelected();
}

int GamepadSelectable::GetSelectedInteger(std::string_view name) const {
  return std::stoi(std::string{GetSelected(name)});
}

double GamepadSelectable::GetSelectedDouble(std::string_view name) const {
  return std::stod(std::string{GetSelected(name)});
}

int GamepadSelectable::GetSelectedIndex(std::string_view name) const {
  return GetChooser(name)->GetSelectedIndex();
}

std::vector<std::string> GamepadSelectable::GetChooserNames() const {
  std::vector<std::string> names;
  names.reserve(m_choosers.size());
  for (const auto& chooser : m_choosers) {
    names.emplace_back(chooser->GetName());
  }
  return names;
}

GamepadSelectable::Chooser* GamepadSelectable::GetSelectedChooser() {
  if (m_choosers.empty()) {
    return nullptr;
  }
  return m_choosers[m_selectedChooser].get();
}

const GamepadSelectable::Chooser* GamepadSelectable::GetSelectedChooser()
    const {
  if (m_choosers.empty()) {
    return nullptr;
  }
  return m_choosers[m_selectedChooser].get();
}

GamepadSelectable::Chooser* GamepadSelectable::GetChooser(
    std::string_view name) {
  return const_cast<Chooser*>(std::as_const(*this).GetChooser(name));
}

const GamepadSelectable::Chooser* GamepadSelectable::GetChooser(
    std::string_view name) const {
  RequireChooserName(name);
  auto it = m_chooserMap.find(std::string{name});
  if (it == m_chooserMap.end()) {
    throw std::invalid_argument("Unknown chooser: " + std::string{name});
  }
  return it->second;
}

void GamepadSelectable::RequireChooserName(std::string_view name) {
  if (name.empty()) {
    throw std::invalid_argument("Chooser name cannot be blank");
  }
  for (char ch : name) {
    if (!std::isspace(static_cast<unsigned char>(ch))) {
      return;
    }
  }
  throw std::invalid_argument("Chooser name cannot be blank");
}

int GamepadSelectable::Wrap(int value, int size) {
  return (value % size + size) % size;
}

std::string GamepadSelectable::FormatDisplayLine(const Chooser& chooser,
                                                 bool selected) {
  if (selected) {
    return "> " + std::string{chooser.GetName()} + " : " +
           std::string{chooser.GetSelected()} + " <";
  }
  return "  " + std::string{chooser.GetName()} + " : " +
         std::string{chooser.GetSelected()};
}
