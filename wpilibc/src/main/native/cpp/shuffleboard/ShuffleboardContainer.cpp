/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/ShuffleboardContainer.h"

#include <wpi/SmallVector.h>
#include <wpi/raw_ostream.h>

#include "frc/shuffleboard/ComplexWidget.h"
#include "frc/shuffleboard/SendableCameraWrapper.h"
#include "frc/shuffleboard/ShuffleboardComponent.h"
#include "frc/shuffleboard/ShuffleboardLayout.h"
#include "frc/shuffleboard/SimpleWidget.h"

using namespace frc;

ShuffleboardContainer::ShuffleboardContainer(const wpi::Twine& title)
    : ShuffleboardValue(title) {}

const std::vector<std::unique_ptr<ShuffleboardComponentBase>>&
ShuffleboardContainer::GetComponents() const {
  return m_components;
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
    auto layout = std::make_unique<ShuffleboardLayout>(*this, type, titleRef);
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

ComplexWidget& ShuffleboardContainer::Add(const wpi::Twine& title,
                                          const cs::VideoSource& video) {
  return Add(title, SendableCameraWrapper::Wrap(video));
}

ComplexWidget& ShuffleboardContainer::Add(Sendable& sendable) {
  if (sendable.GetName().empty()) {
    wpi::outs() << "Sendable must have a name\n";
  }
  return Add(sendable.GetName(), sendable);
}

ComplexWidget& ShuffleboardContainer::Add(const cs::VideoSource& video) {
  return Add(SendableCameraWrapper::Wrap(video));
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
