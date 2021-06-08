// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/ShuffleboardContainer.h"

#include "frc/Errors.h"
#include "frc/shuffleboard/ComplexWidget.h"
#include "frc/shuffleboard/ShuffleboardComponent.h"
#include "frc/shuffleboard/ShuffleboardLayout.h"
#include "frc/shuffleboard/SimpleWidget.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

static constexpr const char* layoutStrings[] = {"List Layout", "Grid Layout"};

static constexpr const char* GetStringFromBuiltInLayout(BuiltInLayouts layout) {
  return layoutStrings[static_cast<int>(layout)];
}

ShuffleboardContainer::ShuffleboardContainer(std::string_view title)
    : ShuffleboardValue(title) {}

const std::vector<std::unique_ptr<ShuffleboardComponentBase>>&
ShuffleboardContainer::GetComponents() const {
  return m_components;
}

ShuffleboardLayout& ShuffleboardContainer::GetLayout(std::string_view title,
                                                     BuiltInLayouts type) {
  return GetLayout(title, GetStringFromBuiltInLayout(type));
}

ShuffleboardLayout& ShuffleboardContainer::GetLayout(std::string_view title,
                                                     const LayoutType& type) {
  return GetLayout(title, type.GetLayoutName());
}

ShuffleboardLayout& ShuffleboardContainer::GetLayout(std::string_view title,
                                                     std::string_view type) {
  if (m_layouts.count(title) == 0) {
    auto layout = std::make_unique<ShuffleboardLayout>(*this, title, type);
    auto ptr = layout.get();
    m_components.emplace_back(std::move(layout));
    m_layouts.insert(std::make_pair(title, ptr));
  }
  return *m_layouts[title];
}

ShuffleboardLayout& ShuffleboardContainer::GetLayout(std::string_view title) {
  if (m_layouts.count(title) == 0) {
    throw FRC_MakeError(err::InvalidParameter,
                        "No layout with title {} has been defined", title);
  }
  return *m_layouts[title];
}

ComplexWidget& ShuffleboardContainer::Add(std::string_view title,
                                          Sendable& sendable) {
  CheckTitle(title);
  auto widget = std::make_unique<ComplexWidget>(*this, title, sendable);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

ComplexWidget& ShuffleboardContainer::Add(Sendable& sendable) {
  auto name = SendableRegistry::GetInstance().GetName(&sendable);
  if (name.empty()) {
    FRC_ReportError(err::Error, "{}", "Sendable must have a name");
  }
  return Add(name, sendable);
}

SimpleWidget& ShuffleboardContainer::Add(
    std::string_view title, std::shared_ptr<nt::Value> defaultValue) {
  CheckTitle(title);

  auto widget = std::make_unique<SimpleWidget>(*this, title);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  ptr->GetEntry().SetDefaultValue(defaultValue);
  return *ptr;
}

SimpleWidget& ShuffleboardContainer::Add(std::string_view title,
                                         bool defaultValue) {
  return Add(title, nt::Value::MakeBoolean(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(std::string_view title,
                                         double defaultValue) {
  return Add(title, nt::Value::MakeDouble(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(std::string_view title,
                                         int defaultValue) {
  return Add(title, nt::Value::MakeDouble(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(std::string_view title,
                                         std::string_view defaultValue) {
  return Add(title, nt::Value::MakeString(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(std::string_view title,
                                         const char* defaultValue) {
  return Add(title, nt::Value::MakeString(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(std::string_view title,
                                         wpi::span<const bool> defaultValue) {
  return Add(title, nt::Value::MakeBooleanArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(std::string_view title,
                                         wpi::span<const double> defaultValue) {
  return Add(title, nt::Value::MakeDoubleArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(
    std::string_view title, wpi::span<const std::string> defaultValue) {
  return Add(title, nt::Value::MakeStringArray(defaultValue));
}

SuppliedValueWidget<std::string>& ShuffleboardContainer::AddString(
    std::string_view title, std::function<std::string()> supplier) {
  static auto setter = [](nt::NetworkTableEntry entry, std::string value) {
    entry.SetString(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<std::string>>(
      *this, title, supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<double>& ShuffleboardContainer::AddNumber(
    std::string_view title, std::function<double()> supplier) {
  static auto setter = [](nt::NetworkTableEntry entry, double value) {
    entry.SetDouble(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<double>>(*this, title,
                                                              supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<bool>& ShuffleboardContainer::AddBoolean(
    std::string_view title, std::function<bool()> supplier) {
  static auto setter = [](nt::NetworkTableEntry entry, bool value) {
    entry.SetBoolean(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<bool>>(*this, title,
                                                            supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<std::vector<std::string>>&
ShuffleboardContainer::AddStringArray(
    std::string_view title,
    std::function<std::vector<std::string>()> supplier) {
  static auto setter = [](nt::NetworkTableEntry entry,
                          std::vector<std::string> value) {
    entry.SetStringArray(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<std::vector<std::string>>>(
      *this, title, supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<std::vector<double>>& ShuffleboardContainer::AddNumberArray(
    std::string_view title, std::function<std::vector<double>()> supplier) {
  static auto setter = [](nt::NetworkTableEntry entry,
                          std::vector<double> value) {
    entry.SetDoubleArray(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<std::vector<double>>>(
      *this, title, supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<std::vector<int>>& ShuffleboardContainer::AddBooleanArray(
    std::string_view title, std::function<std::vector<int>()> supplier) {
  static auto setter = [](nt::NetworkTableEntry entry, std::vector<int> value) {
    entry.SetBooleanArray(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<std::vector<int>>>(
      *this, title, supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<std::string_view>& ShuffleboardContainer::AddRaw(
    std::string_view title, std::function<std::string_view()> supplier) {
  static auto setter = [](nt::NetworkTableEntry entry, std::string_view value) {
    entry.SetRaw(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<std::string_view>>(
      *this, title, supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    std::string_view title, std::shared_ptr<nt::Value> defaultValue) {
  auto& widget = Add(title, defaultValue);
  widget.GetEntry().SetPersistent();
  return widget;
}

SimpleWidget& ShuffleboardContainer::AddPersistent(std::string_view title,
                                                   bool defaultValue) {
  return AddPersistent(title, nt::Value::MakeBoolean(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(std::string_view title,
                                                   double defaultValue) {
  return AddPersistent(title, nt::Value::MakeDouble(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(std::string_view title,
                                                   int defaultValue) {
  return AddPersistent(title, nt::Value::MakeDouble(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    std::string_view title, std::string_view defaultValue) {
  return AddPersistent(title, nt::Value::MakeString(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    std::string_view title, wpi::span<const bool> defaultValue) {
  return AddPersistent(title, nt::Value::MakeBooleanArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    std::string_view title, wpi::span<const double> defaultValue) {
  return AddPersistent(title, nt::Value::MakeDoubleArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    std::string_view title, wpi::span<const std::string> defaultValue) {
  return AddPersistent(title, nt::Value::MakeStringArray(defaultValue));
}

void ShuffleboardContainer::EnableIfActuator() {
  for (auto& component : GetComponents()) {
    component->EnableIfActuator();
  }
}

void ShuffleboardContainer::DisableIfActuator() {
  for (auto& component : GetComponents()) {
    component->DisableIfActuator();
  }
}

void ShuffleboardContainer::CheckTitle(std::string_view title) {
  std::string titleStr{title};
  if (m_usedTitles.count(titleStr) > 0) {
    FRC_ReportError(err::Error, "Title is already in use: {}", title);
    return;
  }
  m_usedTitles.insert(titleStr);
}
