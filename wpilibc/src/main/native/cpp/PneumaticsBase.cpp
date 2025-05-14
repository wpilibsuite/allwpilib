// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PneumaticsBase.h"

#include <memory>

#include <hal/REVPH.h>

#include "frc/Errors.h"
#include "frc/PneumaticHub.h"
#include "frc/PneumaticsControlModule.h"
#include "frc/SensorUtil.h"

using namespace frc;

static_assert(
    static_cast<int>(CompressorConfigType::Disabled) ==
    HAL_REVPHCompressorConfigType::HAL_REVPHCompressorConfigType_Disabled);
static_assert(
    static_cast<int>(CompressorConfigType::Digital) ==
    HAL_REVPHCompressorConfigType::HAL_REVPHCompressorConfigType_Digital);
static_assert(
    static_cast<int>(CompressorConfigType::Analog) ==
    HAL_REVPHCompressorConfigType::HAL_REVPHCompressorConfigType_Analog);
static_assert(
    static_cast<int>(CompressorConfigType::Hybrid) ==
    HAL_REVPHCompressorConfigType::HAL_REVPHCompressorConfigType_Hybrid);

std::shared_ptr<PneumaticsBase> PneumaticsBase::GetForType(
    int busId, int module, PneumaticsModuleType moduleType) {
  if (moduleType == PneumaticsModuleType::CTREPCM) {
    return PneumaticsControlModule::GetForModule(busId, module);
  } else if (moduleType == PneumaticsModuleType::REVPH) {
    return PneumaticHub::GetForModule(busId, module);
  }
  throw FRC_MakeError(err::InvalidParameter, "{}",
                      static_cast<int>(moduleType));
}

int PneumaticsBase::GetDefaultForType(PneumaticsModuleType moduleType) {
  if (moduleType == PneumaticsModuleType::CTREPCM) {
    return SensorUtil::GetDefaultCTREPCMModule();
  } else if (moduleType == PneumaticsModuleType::REVPH) {
    return SensorUtil::GetDefaultREVPHModule();
  }
  throw FRC_MakeError(err::InvalidParameter, "{}",
                      static_cast<int>(moduleType));
}
