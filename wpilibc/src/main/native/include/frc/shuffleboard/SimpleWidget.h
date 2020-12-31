// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <wpi/Twine.h>

#include "frc/shuffleboard/ShuffleboardWidget.h"

namespace frc {

class ShuffleboardContainer;

/**
 * A Shuffleboard widget that handles a single data point such as a number or
 * string.
 */
class SimpleWidget final : public ShuffleboardWidget<SimpleWidget> {
 public:
  SimpleWidget(ShuffleboardContainer& parent, const wpi::Twine& title);

  /**
   * Gets the NetworkTable entry that contains the data for this widget.
   */
  nt::NetworkTableEntry GetEntry();

  void BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                 std::shared_ptr<nt::NetworkTable> metaTable) override;

 private:
  nt::NetworkTableEntry m_entry;

  void ForceGenerate();
};

}  // namespace frc
