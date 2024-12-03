// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/ShuffleboardComponentBase.h"

#include <memory>
#include <string>

using namespace frc;

ShuffleboardComponentBase::ShuffleboardComponentBase(
    ShuffleboardContainer& parent, std::string_view title,
    std::string_view type)
    : ShuffleboardValue(title), m_parent(parent), m_type(type) {}

void ShuffleboardComponentBase::SetType(std::string_view type) {
  m_type = type;
  m_metadataDirty = true;
}

void ShuffleboardComponentBase::BuildMetadata(
    std::shared_ptr<nt::NetworkTable> metaTable) {
  if (!m_metadataDirty) {
    return;
  }
  // Component type
  if (!GetType().empty()) {
    metaTable->GetEntry("PreferredComponent").SetString(GetType());
  }

  // Tile size
  if (m_width > 0 && m_height > 0) {
    metaTable->GetEntry("Size").SetDoubleArray(
        {{static_cast<double>(m_width), static_cast<double>(m_height)}});
  }

  // Tile position
  if (m_column >= 0 && m_row >= 0) {
    metaTable->GetEntry("Position")
        .SetDoubleArray(
            {{static_cast<double>(m_column), static_cast<double>(m_row)}});
  }

  // Custom properties
  if (GetProperties().size() > 0) {
    auto propTable = metaTable->GetSubTable("Properties");
    for (auto& entry : GetProperties()) {
      propTable->GetEntry(entry.first).SetValue(entry.second);
    }
  }
  m_metadataDirty = false;
}

ShuffleboardContainer& ShuffleboardComponentBase::GetParent() {
  return m_parent;
}

const std::string& ShuffleboardComponentBase::GetType() const {
  return m_type;
}

const wpi::StringMap<nt::Value>& ShuffleboardComponentBase::GetProperties()
    const {
  return m_properties;
}
