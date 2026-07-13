// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/DSGamepadChooser.hpp"

#include <atomic>
#include <cctype>
#include <cmath>
#include <cstdint>
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

DSGamepadChooser::GamepadSelectable::GamepadSelectable(
    std::string_view name, std::vector<std::string> options)
    : m_name{name}, m_options{std::move(options)} {}

std::string_view DSGamepadChooser::GamepadSelectable::GetName() const {
  return m_name;
}

const std::vector<std::string>&
DSGamepadChooser::GamepadSelectable::GetOptions() const {
  return m_options;
}

std::string_view DSGamepadChooser::GamepadSelectable::GetSelected() const {
  return m_options[m_selectedIndex];
}

int DSGamepadChooser::GamepadSelectable::GetSelectedIndex() const {
  return m_selectedIndex;
}

void DSGamepadChooser::GamepadSelectable::SelectPrevious() {
  m_selectedIndex =
      Wrap(m_selectedIndex - 1, static_cast<int>(m_options.size()));
}

void DSGamepadChooser::GamepadSelectable::SelectNext() {
  m_selectedIndex =
      Wrap(m_selectedIndex + 1, static_cast<int>(m_options.size()));
}

DSGamepadChooser::DSGamepadChooser(int port)
    : DSGamepadChooser{DriverStation::GetGamepad(port)} {}

DSGamepadChooser::DSGamepadChooser(Gamepad& gamepad)
    : m_gamepad{&gamepad},
      m_captionPrefix{"DSGamepadChooser/" + std::to_string(instances++) + "/"} {
}

Gamepad& DSGamepadChooser::GetGamepad() {
  return *m_gamepad;
}

const Gamepad& DSGamepadChooser::GetGamepad() const {
  return *m_gamepad;
}

DSGamepadChooser::GamepadSelectable& DSGamepadChooser::AddOptions(
    std::string_view name, std::vector<std::string> options) {
  RequireSelectableName(name);
  if (options.empty()) {
    throw std::invalid_argument("Options cannot be empty");
  }

  std::string nameString{name};
  if (m_selectableMap.contains(nameString)) {
    throw std::invalid_argument("GamepadSelectable already exists: " +
                                nameString);
  }

  auto selectable = std::unique_ptr<GamepadSelectable>{
      new GamepadSelectable{nameString, std::move(options)}};
  auto& selectableRef = *selectable;
  m_selectableMap.emplace(std::move(nameString), selectable.get());
  m_selectables.emplace_back(std::move(selectable));
  return selectableRef;
}

DSGamepadChooser::GamepadSelectable& DSGamepadChooser::AddOptions(
    std::string_view name, std::initializer_list<std::string_view> options) {
  std::vector<std::string> optionStrings;
  optionStrings.reserve(options.size());
  for (auto option : options) {
    optionStrings.emplace_back(option);
  }
  return AddOptions(name, std::move(optionStrings));
}

DSGamepadChooser::GamepadSelectable& DSGamepadChooser::AddIntegerOptions(
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

DSGamepadChooser::GamepadSelectable& DSGamepadChooser::AddDoubleOptions(
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
      throw std::invalid_argument(
          "Delta is too small to produce another option");
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

void DSGamepadChooser::Update() {
  if (m_selectables.empty()) {
    return;
  }

  if (m_gamepad->GetDpadUpButtonPressed()) {
    m_selectedSelectable =
        Wrap(m_selectedSelectable - 1, static_cast<int>(m_selectables.size()));
  }
  if (m_gamepad->GetDpadDownButtonPressed()) {
    m_selectedSelectable =
        Wrap(m_selectedSelectable + 1, static_cast<int>(m_selectables.size()));
  }

  GamepadSelectable& selectable = *m_selectables[m_selectedSelectable];
  if (m_gamepad->GetDpadLeftButtonPressed()) {
    selectable.SelectPrevious();
  }
  if (m_gamepad->GetDpadRightButtonPressed()) {
    selectable.SelectNext();
  }

  for (size_t i = 0; i < m_selectables.size(); i++) {
    const GamepadSelectable& displaySelectable = *m_selectables[i];
    DriverStationDisplay::AddData(
        m_captionPrefix + std::string{displaySelectable.GetName()},
        FormatDisplayLine(displaySelectable,
                          i == static_cast<size_t>(m_selectedSelectable)));
  }
}

std::string_view DSGamepadChooser::GetSelected(std::string_view name) const {
  return GetSelectable(name)->GetSelected();
}

int DSGamepadChooser::GetSelectedInteger(std::string_view name) const {
  return std::stoi(std::string{GetSelected(name)});
}

double DSGamepadChooser::GetSelectedDouble(std::string_view name) const {
  return std::stod(std::string{GetSelected(name)});
}

int DSGamepadChooser::GetSelectedIndex(std::string_view name) const {
  return GetSelectable(name)->GetSelectedIndex();
}

std::vector<std::string> DSGamepadChooser::GetSelectableNames() const {
  std::vector<std::string> names;
  names.reserve(m_selectables.size());
  for (const auto& selectable : m_selectables) {
    names.emplace_back(selectable->GetName());
  }
  return names;
}

DSGamepadChooser::GamepadSelectable* DSGamepadChooser::GetSelectedSelectable() {
  if (m_selectables.empty()) {
    return nullptr;
  }
  return m_selectables[m_selectedSelectable].get();
}

const DSGamepadChooser::GamepadSelectable*
DSGamepadChooser::GetSelectedSelectable() const {
  if (m_selectables.empty()) {
    return nullptr;
  }
  return m_selectables[m_selectedSelectable].get();
}

DSGamepadChooser::GamepadSelectable* DSGamepadChooser::GetSelectable(
    std::string_view name) {
  return const_cast<GamepadSelectable*>(
      std::as_const(*this).GetSelectable(name));
}

const DSGamepadChooser::GamepadSelectable* DSGamepadChooser::GetSelectable(
    std::string_view name) const {
  RequireSelectableName(name);
  auto it = m_selectableMap.find(std::string{name});
  if (it == m_selectableMap.end()) {
    throw std::invalid_argument("Unknown selectable: " + std::string{name});
  }
  return it->second;
}

void DSGamepadChooser::RequireSelectableName(std::string_view name) {
  if (name.empty()) {
    throw std::invalid_argument("GamepadSelectable name cannot be blank");
  }
  for (char ch : name) {
    if (!std::isspace(static_cast<unsigned char>(ch))) {
      return;
    }
  }
  throw std::invalid_argument("GamepadSelectable name cannot be blank");
}

int DSGamepadChooser::Wrap(int value, int size) {
  return (value % size + size) % size;
}

std::string DSGamepadChooser::FormatDisplayLine(
    const GamepadSelectable& selectable, bool selected) {
  if (selected) {
    return "> " + std::string{selectable.GetName()} + " : " +
           std::string{selectable.GetSelected()} + " <";
  }
  return "  " + std::string{selectable.GetName()} + " : " +
         std::string{selectable.GetSelected()};
}
