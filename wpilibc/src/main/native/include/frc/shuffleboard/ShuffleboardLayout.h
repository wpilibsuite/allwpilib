/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <networktables/NetworkTable.h>
#include <wpi/Twine.h>

#include "frc/shuffleboard/ShuffleboardComponent.h"
#include "frc/shuffleboard/ShuffleboardContainer.h"
#include "frc/smartdashboard/Sendable.h"

namespace frc {

/**
 * A layout in a Shuffleboard tab. Layouts can contain widgets and other
 * layouts.
 */
class ShuffleboardLayout : public ShuffleboardComponent<ShuffleboardLayout>,
                           public ShuffleboardContainer {
 public:
  ShuffleboardLayout(ShuffleboardContainer& parent, const wpi::Twine& name,
                     const wpi::Twine& type);

  void BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                 std::shared_ptr<nt::NetworkTable> metaTable) override;
};

}  // namespace frc
