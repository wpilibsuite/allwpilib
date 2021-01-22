// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableValue.h>
#include <wpi/StringMap.h>
#include <wpi/Twine.h>

#include "frc/shuffleboard/ShuffleboardValue.h"

namespace frc {

class ShuffleboardContainer;

/**
 * A shim class to allow storing ShuffleboardComponents in arrays.
 */
class ShuffleboardComponentBase : public virtual ShuffleboardValue {
 public:
  ShuffleboardComponentBase(ShuffleboardContainer& parent,
                            const wpi::Twine& title,
                            const wpi::Twine& type = "");

  ~ShuffleboardComponentBase() override = default;

  void SetType(const wpi::Twine& type);

  void BuildMetadata(std::shared_ptr<nt::NetworkTable> metaTable);

  ShuffleboardContainer& GetParent();

  const std::string& GetType() const;

 protected:
  wpi::StringMap<std::shared_ptr<nt::Value>> m_properties;
  bool m_metadataDirty = true;
  int m_column = -1;
  int m_row = -1;
  int m_width = -1;
  int m_height = -1;

 private:
  ShuffleboardContainer& m_parent;
  std::string m_type;

  /**
   * Gets the custom properties for this component. May be null.
   */
  const wpi::StringMap<std::shared_ptr<nt::Value>>& GetProperties() const;
};

}  // namespace frc
