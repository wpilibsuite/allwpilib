/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/ShuffleboardContainer.h"

#include <wpi/SmallVector.h>
#include <wpi/raw_ostream.h>

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

ShuffleboardContainer::ShuffleboardContainer(const wpi::Twine& title)
    : ShuffleboardValue(title) {}

const std::vector<std::unique_ptr<ShuffleboardComponentBase>>&
ShuffleboardContainer::GetComponents() const {
  return m_components;
}

ShuffleboardLayout& ShuffleboardContainer::GetLayout(const wpi::Twine& title,
                                                     BuiltInLayouts type) {
  return GetLayout(title, GetStringFromBuiltInLayout(type));
}

ShuffleboardLayout& ShuffleboardContainer::GetLayout(const wpi::Twine& title,
                                                     const LayoutType& type) {
  return GetLayout(title, type.GetLayoutName());
}

ShuffleboardLayout& ShuffleboardContainer::GetLayout(const wpi::Twine& title,
                                                     const wpi::Twine& type) {
  wpi::SmallVector<char, 16> storage;
  auto titleRef = title.toStringRef(storage);
  if (m_layouts.count(titleRef) == 0) {
    auto layout = std::make_unique<ShuffleboardLayout>(*this, titleRef, type);
    auto ptr = layout.get();
    m_components.emplace_back(std::move(layout));
    m_layouts.insert(std::make_pair(titleRef, ptr));
  }
  return *m_layouts[titleRef];
}

ShuffleboardLayout& ShuffleboardContainer::GetLayout(const wpi::Twine& title) {
  wpi::SmallVector<char, 16> storage;
  auto titleRef = title.toStringRef(storage);
  if (m_layouts.count(titleRef) == 0) {
    wpi_setWPIErrorWithContext(
        InvalidParameter, "No layout with the given title has been defined");
  }
  return *m_layouts[titleRef];
}

ComplexWidget& ShuffleboardContainer::Add(const wpi::Twine& title,
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
    wpi::outs() << "Sendable must have a name\n";
  }
  return Add(name, sendable);
}

SimpleWidget& ShuffleboardContainer::Add(
    const wpi::Twine& title, std::shared_ptr<nt::Value> defaultValue) {
  CheckTitle(title);

  auto widget = std::make_unique<SimpleWidget>(*this, title);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  ptr->GetEntry().SetDefaultValue(defaultValue);
  return *ptr;
}

SimpleWidget& ShuffleboardContainer::Add(const wpi::Twine& title,
                                         bool defaultValue) {
  return Add(title, nt::Value::MakeBoolean(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(const wpi::Twine& title,
                                         double defaultValue) {
  return Add(title, nt::Value::MakeDouble(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(const wpi::Twine& title,
                                         int defaultValue) {
  return Add(title, nt::Value::MakeDouble(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(const wpi::Twine& title,
                                         const wpi::Twine& defaultValue) {
  return Add(title, nt::Value::MakeString(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(const wpi::Twine& title,
                                         const char* defaultValue) {
  return Add(title, nt::Value::MakeString(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(const wpi::Twine& title,
                                         wpi::ArrayRef<bool> defaultValue) {
  return Add(title, nt::Value::MakeBooleanArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(const wpi::Twine& title,
                                         wpi::ArrayRef<double> defaultValue) {
  return Add(title, nt::Value::MakeDoubleArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::Add(
    const wpi::Twine& title, wpi::ArrayRef<std::string> defaultValue) {
  return Add(title, nt::Value::MakeStringArray(defaultValue));
}

SuppliedValueWidget<std::string>& ShuffleboardContainer::AddString(
    const wpi::Twine& title, std::function<std::string()> supplier) {
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
    const wpi::Twine& title, std::function<double()> supplier) {
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
    const wpi::Twine& title, std::function<bool()> supplier) {
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
    const wpi::Twine& title,
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
    const wpi::Twine& title, std::function<std::vector<double>()> supplier) {
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
    const wpi::Twine& title, std::function<std::vector<int>()> supplier) {
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

SuppliedValueWidget<wpi::StringRef>& ShuffleboardContainer::AddRaw(
    const wpi::Twine& title, std::function<wpi::StringRef()> supplier) {
  static auto setter = [](nt::NetworkTableEntry entry, wpi::StringRef value) {
    entry.SetRaw(value);
  };

  CheckTitle(title);
  auto widget = std::make_unique<SuppliedValueWidget<wpi::StringRef>>(
      *this, title, supplier, setter);
  auto ptr = widget.get();
  m_components.emplace_back(std::move(widget));
  return *ptr;
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    const wpi::Twine& title, std::shared_ptr<nt::Value> defaultValue) {
  auto& widget = Add(title, defaultValue);
  widget.GetEntry().SetPersistent();
  return widget;
}

SimpleWidget& ShuffleboardContainer::AddPersistent(const wpi::Twine& title,
                                                   bool defaultValue) {
  return AddPersistent(title, nt::Value::MakeBoolean(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(const wpi::Twine& title,
                                                   double defaultValue) {
  return AddPersistent(title, nt::Value::MakeDouble(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(const wpi::Twine& title,
                                                   int defaultValue) {
  return AddPersistent(title, nt::Value::MakeDouble(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    const wpi::Twine& title, const wpi::Twine& defaultValue) {
  return AddPersistent(title, nt::Value::MakeString(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    const wpi::Twine& title, wpi::ArrayRef<bool> defaultValue) {
  return AddPersistent(title, nt::Value::MakeBooleanArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    const wpi::Twine& title, wpi::ArrayRef<double> defaultValue) {
  return AddPersistent(title, nt::Value::MakeDoubleArray(defaultValue));
}

SimpleWidget& ShuffleboardContainer::AddPersistent(
    const wpi::Twine& title, wpi::ArrayRef<std::string> defaultValue) {
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

void ShuffleboardContainer::CheckTitle(const wpi::Twine& title) {
  wpi::SmallVector<char, 16> storage;
  auto titleRef = title.toStringRef(storage);
  if (m_usedTitles.count(titleRef) > 0) {
    wpi::errs() << "Title is already in use: " << title << "\n";
    return;
  }
  m_usedTitles.insert(titleRef);
}
