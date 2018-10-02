/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  virtual ~ShuffleboardComponentBase() = default;

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
