// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/ShuffleboardContainer.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ntcore_cpp.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/shuffleboard/ComplexWidget.h"
#include "frc/shuffleboard/ShuffleboardComponent.h"
#include "frc/shuffleboard/ShuffleboardLayout.h"
#include "frc/shuffleboard/SimpleWidget.h"

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
    m_layouts.insert(std::pair{title, ptr});
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
                                          wpi::Sendable& sendable) {
  CheckTitle(title);
  auto widget = std::make_unique<ComplexWidget>(*this, title, sendable);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

ComplexWidget& ShuffleboardContainer::Add(wpi::Sendable& sendable) {
  auto name = wpi::SendableRegistry::GetName(&sendable);
  if (name.empty()) {
    FRC_ReportError(err::Error, "Sendable must have a name");
  }
  return Add(name, sendable);
}

SimpleWidget& ShuffleboardContainer::Add(std::string_view title,
                                         const nt::Value& defaultValue) {
  CheckTitle(title);

  auto widget = std::make_unique<SimpleWidget>(*this, title);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  ptr->GetEntry(nt::GetStringFromType(defaultValue.type()))
      ->SetDefault(defaultValue);
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
                                         float defaultValue) {
  return Add(title, nt::Value::MakeFloat(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(std::string_view title,
                                         int defaultValue) {
  return Add(title, nt::Value::MakeInteger(defaultValue));
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
                                         std::span<const bool> defaultValue) {
  return Add(title, nt::Value::MakeBooleanArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(std::string_view title,
                                         std::span<const double> defaultValue) {
  return Add(title, nt::Value::MakeDoubleArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(std::string_view title,
                                         std::span<const float> defaultValue) {
  return Add(title, nt::Value::MakeFloatArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(
    std::string_view title, std::span<const int64_t> defaultValue) {
  return Add(title, nt::Value::MakeIntegerArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(
    std::string_view title, std::span<const std::string> defaultValue) {
  return Add(title, nt::Value::MakeStringArray(defaultValue));
}

SuppliedValueWidget<std::string>& ShuffleboardContainer::AddString(
    std::string_view title, std::function<std::string()> supplier) {
  static auto setter = [](nt::GenericPublisher& entry, std::string value) {
    entry.SetString(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<std::string>>(
      *this, title, "string", supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<double>& ShuffleboardContainer::AddNumber(
    std::string_view title, std::function<double()> supplier) {
  return AddDouble(title, std::move(supplier));
}

SuppliedValueWidget<double>& ShuffleboardContainer::AddDouble(
    std::string_view title, std::function<double()> supplier) {
  static auto setter = [](nt::GenericPublisher& entry, double value) {
    entry.SetDouble(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<double>>(
      *this, title, "double", supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<float>& ShuffleboardContainer::AddFloat(
    std::string_view title, std::function<float()> supplier) {
  static auto setter = [](nt::GenericPublisher& entry, float value) {
    entry.SetFloat(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<float>>(
      *this, title, "float", supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<int64_t>& ShuffleboardContainer::AddInteger(
    std::string_view title, std::function<int64_t()> supplier) {
  static auto setter = [](nt::GenericPublisher& entry, int64_t value) {
    entry.SetInteger(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<int64_t>>(
      *this, title, "int", supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<bool>& ShuffleboardContainer::AddBoolean(
    std::string_view title, std::function<bool()> supplier) {
  static auto setter = [](nt::GenericPublisher& entry, bool value) {
    entry.SetBoolean(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<bool>>(
      *this, title, "boolean", supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<std::vector<std::string>>&
ShuffleboardContainer::AddStringArray(
    std::string_view title,
    std::function<std::vector<std::string>()> supplier) {
  static auto setter = [](nt::GenericPublisher& entry,
                          std::vector<std::string> value) {
    entry.SetStringArray(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<std::vector<std::string>>>(
      *this, title, "string[]", supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<std::vector<double>>& ShuffleboardContainer::AddNumberArray(
    std::string_view title, std::function<std::vector<double>()> supplier) {
  return AddDoubleArray(title, std::move(supplier));
}

SuppliedValueWidget<std::vector<double>>& ShuffleboardContainer::AddDoubleArray(
    std::string_view title, std::function<std::vector<double>()> supplier) {
  static auto setter = [](nt::GenericPublisher& entry,
                          std::vector<double> value) {
    entry.SetDoubleArray(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<std::vector<double>>>(
      *this, title, "double[]", supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<std::vector<float>>& ShuffleboardContainer::AddFloatArray(
    std::string_view title, std::function<std::vector<float>()> supplier) {
  static auto setter = [](nt::GenericPublisher& entry,
                          std::vector<float> value) {
    entry.SetFloatArray(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<std::vector<float>>>(
      *this, title, "float[]", supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<std::vector<int64_t>>&
ShuffleboardContainer::AddIntegerArray(
    std::string_view title, std::function<std::vector<int64_t>()> supplier) {
  static auto setter = [](nt::GenericPublisher& entry,
                          std::vector<int64_t> value) {
    entry.SetIntegerArray(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<std::vector<int64_t>>>(
      *this, title, "int[]", supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<std::vector<int>>& ShuffleboardContainer::AddBooleanArray(
    std::string_view title, std::function<std::vector<int>()> supplier) {
  static auto setter = [](nt::GenericPublisher& entry, std::vector<int> value) {
    entry.SetBooleanArray(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<std::vector<int>>>(
      *this, title, "boolean[]", supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SuppliedValueWidget<std::vector<uint8_t>>& ShuffleboardContainer::AddRaw(
    std::string_view title, std::function<std::vector<uint8_t>()> supplier) {
  return AddRaw(title, "raw", std::move(supplier));
}

SuppliedValueWidget<std::vector<uint8_t>>& ShuffleboardContainer::AddRaw(
    std::string_view title, std::string_view typeString,
    std::function<std::vector<uint8_t>()> supplier) {
  static auto setter = [](nt::GenericPublisher& entry,
                          std::vector<uint8_t> value) { entry.SetRaw(value); };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<std::vector<uint8_t>>>(
      *this, title, typeString, supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    std::string_view title, const nt::Value& defaultValue) {
  auto& widget = Add(title, defaultValue);
  widget.GetEntry(nt::GetStringFromType(defaultValue.type()))
      ->GetTopic()
      .SetPersistent(true);
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
                                                   float defaultValue) {
  return AddPersistent(title, nt::Value::MakeFloat(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(std::string_view title,
                                                   int defaultValue) {
  return AddPersistent(title, nt::Value::MakeInteger(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    std::string_view title, std::string_view defaultValue) {
  return AddPersistent(title, nt::Value::MakeString(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    std::string_view title, std::span<const bool> defaultValue) {
  return AddPersistent(title, nt::Value::MakeBooleanArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    std::string_view title, std::span<const double> defaultValue) {
  return AddPersistent(title, nt::Value::MakeDoubleArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    std::string_view title, std::span<const float> defaultValue) {
  return AddPersistent(title, nt::Value::MakeFloatArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    std::string_view title, std::span<const int64_t> defaultValue) {
  return AddPersistent(title, nt::Value::MakeIntegerArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    std::string_view title, std::span<const std::string> defaultValue) {
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
