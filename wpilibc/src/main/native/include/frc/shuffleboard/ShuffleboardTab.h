// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include <networktables/NetworkTable.h>

#include "frc/shuffleboard/ShuffleboardContainer.h"

namespace frc {

class ShuffleboardRoot;

/**
 * Represents a tab in the Shuffleboard dashboard. Widgets can be added to the
 * tab with Add(Sendable), Add(std::string_view, Object), and
 * Add(String, Sendable). Widgets can also be added to layouts with
 * GetLayout(std::string_view, std::string_view); layouts can be nested
 * arbitrarily deep (note that too many levels may make deeper components
 * unusable).
 */
class ShuffleboardTab final : public ShuffleboardContainer {
 public:
  ShuffleboardTab(ShuffleboardRoot& root, std::string_view title);

  ShuffleboardRoot& GetRoot();

  void BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                 std::shared_ptr<nt::NetworkTable> metaTable) override;

 private:
  ShuffleboardRoot& m_root;
};

}  // namespace frc
