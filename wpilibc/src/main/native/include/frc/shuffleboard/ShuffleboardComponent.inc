// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "frc/shuffleboard/ShuffleboardComponent.h"

namespace frc {

template <typename Derived>
ShuffleboardComponent<Derived>::ShuffleboardComponent(
    ShuffleboardContainer& parent, std::string_view title,
    std::string_view type)
    : ShuffleboardValue(title),
      ShuffleboardComponentBase(parent, title, type) {}

template <typename Derived>
Derived& ShuffleboardComponent<Derived>::WithProperties(
    const wpi::StringMap<nt::Value>& properties) {
  m_properties = properties;
  m_metadataDirty = true;
  return *static_cast<Derived*>(this);
}

template <typename Derived>
Derived& ShuffleboardComponent<Derived>::WithPosition(int columnIndex,
                                                      int rowIndex) {
  m_column = columnIndex;
  m_row = rowIndex;
  m_metadataDirty = true;
  return *static_cast<Derived*>(this);
}

template <typename Derived>
Derived& ShuffleboardComponent<Derived>::WithSize(int width, int height) {
  m_width = width;
  m_height = height;
  m_metadataDirty = true;
  return *static_cast<Derived*>(this);
}

}  // namespace frc
