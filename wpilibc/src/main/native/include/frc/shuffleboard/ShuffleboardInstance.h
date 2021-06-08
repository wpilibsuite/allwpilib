// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "frc/shuffleboard/ShuffleboardRoot.h"
#include "frc/shuffleboard/ShuffleboardTab.h"

namespace frc::detail {

class ShuffleboardInstance final : public ShuffleboardRoot {
 public:
  explicit ShuffleboardInstance(nt::NetworkTableInstance ntInstance);
  virtual ~ShuffleboardInstance();

  ShuffleboardInstance(ShuffleboardInstance&&) = default;
  ShuffleboardInstance& operator=(ShuffleboardInstance&&) = default;

  frc::ShuffleboardTab& GetTab(std::string_view title) override;

  void Update() override;

  void EnableActuatorWidgets() override;

  void DisableActuatorWidgets() override;

  void SelectTab(int index) override;

  void SelectTab(std::string_view) override;

 private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace frc::detail
