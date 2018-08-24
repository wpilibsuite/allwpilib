/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include "lowfisim/SimulatorComponent.h"

namespace frc {
namespace sim {
namespace lowfi {

class SimulatorComponentBase : public virtual SimulatorComponent {
 public:
  SimulatorComponentBase() = default;
  virtual ~SimulatorComponentBase() = default;

  const std::string& GetDisplayName() const override;

  void SetDisplayName(const std::string& displayName) override;

 private:
  std::string m_name;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
