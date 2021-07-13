// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PneumaticsBase.h"

#include "frc/Errors.h"
#include "frc/PneumaticsControlModule.h"
#include "frc/SensorUtil.h"

using namespace frc;

std::shared_ptr<PneumaticsBase> PneumaticsBase::GetForType(
    int module, PneumaticsModuleType moduleType) {
  if (moduleType == PneumaticsModuleType::CTREPCM) {
    return PneumaticsControlModule::GetForModule(module);
  }
  throw FRC_MakeError(err::InvalidParameter, "{}", moduleType);
}

int PneumaticsBase::GetDefaultForType(PneumaticsModuleType moduleType) {
  if (moduleType == PneumaticsModuleType::CTREPCM) {
    return SensorUtil::GetDefaultCTREPCMModule();
  }
  throw FRC_MakeError(err::InvalidParameter, "{}", moduleType);
}
