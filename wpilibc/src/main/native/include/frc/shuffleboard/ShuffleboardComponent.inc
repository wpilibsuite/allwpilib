/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

namespace frc {

template <typename Derived>
ShuffleboardComponent<Derived>::ShuffleboardComponent(
    ShuffleboardContainer& parent, const wpi::Twine& title,
    const wpi::Twine& type)
    : ShuffleboardValue(title),
      ShuffleboardComponentBase(parent, title, type) {}

template <typename Derived>
Derived& ShuffleboardComponent<Derived>::WithProperties(
    const wpi::StringMap<std::shared_ptr<nt::Value>>& properties) {
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
