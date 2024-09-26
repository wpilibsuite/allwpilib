// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/PneumaticsBaseSim.h"

#include <memory>

#include "frc/Errors.h"
#include "frc/PneumaticsModuleType.h"
#include "frc/simulation/CTREPCMSim.h"
#include "frc/simulation/REVPHSim.h"

using namespace frc;
using namespace frc::sim;

std::shared_ptr<PneumaticsBaseSim> PneumaticsBaseSim::GetForType(
    int module, PneumaticsModuleType type) {
  switch (type) {
    case PneumaticsModuleType::REVPH:
      return std::make_shared<REVPHSim>(module);

    case PneumaticsModuleType::CTREPCM:
      return std::make_shared<CTREPCMSim>(module);

    default:
      throw FRC_MakeError(err::InvalidParameter, "{}",
                          static_cast<int>(module));
  }
}

PneumaticsBaseSim::PneumaticsBaseSim(int module) : m_index{module} {}

PneumaticsBaseSim::PneumaticsBaseSim(const PneumaticsBase& module)
    : m_index{module.GetModuleNumber()} {}
