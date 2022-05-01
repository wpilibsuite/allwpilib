// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/DoubleSolenoid.h"
#include "frc/PneumaticsModuleType.h"
#include "frc/simulation/PneumaticsBaseSim.h"

namespace frc::sim {

class DoubleSolenoidSim {
 public:
  DoubleSolenoidSim(std::shared_ptr<PneumaticsBaseSim> moduleSim, int fwd,
                    int rev);
  DoubleSolenoidSim(int module, PneumaticsModuleType type, int fwd, int rev);
  DoubleSolenoidSim(PneumaticsModuleType type, int fwd, int rev);

  DoubleSolenoid::Value Get() const;
  void Set(DoubleSolenoid::Value output);

  const std::shared_ptr<PneumaticsBaseSim> GetModuleSim() const;

 private:
  const std::shared_ptr<PneumaticsBaseSim> m_module;
  const int m_fwd;
  const int m_rev;
};

}  // namespace frc::sim
