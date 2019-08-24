/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <networktables/NetworkTable.h>
#include <wpi/StringRef.h>

#include "frc/shuffleboard/ShuffleboardContainer.h"

namespace frc {

class ShuffleboardRoot;

/**
 * Represents a tab in the Shuffleboard dashboard. Widgets can be added to the
 * tab with {@link #add(Sendable)}, {@link #add(String, Object)}, and
 * {@link #add(String, Sendable)}. Widgets can also be added to layouts with
 * {@link #getLayout(String, String)}; layouts can be nested arbitrarily deep
 * (note that too many levels may make deeper components unusable).
 */
class ShuffleboardTab final : public ShuffleboardContainer {
 public:
  ShuffleboardTab(ShuffleboardRoot& root, wpi::StringRef title);

  ShuffleboardRoot& GetRoot();

  void BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                 std::shared_ptr<nt::NetworkTable> metaTable) override;

 private:
  ShuffleboardRoot& m_root;
};

}  // namespace frc
