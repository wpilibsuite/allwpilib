/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
