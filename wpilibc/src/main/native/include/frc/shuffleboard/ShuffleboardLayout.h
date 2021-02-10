// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <networktables/NetworkTable.h>
#include <wpi/Twine.h>

#include "frc/shuffleboard/ShuffleboardComponent.h"
#include "frc/shuffleboard/ShuffleboardContainer.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

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

#ifdef _WIN32
#pragma warning(pop)
#endif
