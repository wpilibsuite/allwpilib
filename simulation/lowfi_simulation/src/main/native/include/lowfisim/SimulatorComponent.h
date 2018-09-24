/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

namespace frc {
namespace sim {
namespace lowfi {

class SimulatorComponent {
 public:
  virtual ~SimulatorComponent() = default;

  virtual bool IsWrapperInitialized() const = 0;

  virtual const std::string& GetDisplayName() const = 0;

  virtual void SetDisplayName(const std::string& displayName) = 0;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
