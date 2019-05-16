/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "frc/shuffleboard/ShuffleboardRoot.h"
#include "frc/shuffleboard/ShuffleboardTab.h"

namespace frc {
namespace detail {

class ShuffleboardInstance final : public ShuffleboardRoot {
 public:
  explicit ShuffleboardInstance(nt::NetworkTableInstance ntInstance);
  virtual ~ShuffleboardInstance();

  frc::ShuffleboardTab& GetTab(wpi::StringRef title) override;

  void Update() override;

  void EnableActuatorWidgets() override;

  void DisableActuatorWidgets() override;

  void SelectTab(int index) override;

  void SelectTab(wpi::StringRef) override;

 private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace detail
}  // namespace frc
