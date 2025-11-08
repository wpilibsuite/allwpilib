// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/pneumatic/PneumaticsBase.hpp"

#include <memory>

#include "wpi/hal/REVPH.h"
#include "wpi/hardware/pneumatic/PneumaticHub.hpp"
#include "wpi/hardware/pneumatic/PneumaticsControlModule.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/util/SensorUtil.hpp"

using namespace wpi;

static_assert(
    static_cast<int>(CompressorConfigType::Disabled) ==
    HAL_REVPHCompressorConfigType::HAL_REVPHCompressorConfigType_kDisabled);
static_assert(
    static_cast<int>(CompressorConfigType::Digital) ==
    HAL_REVPHCompressorConfigType::HAL_REVPHCompressorConfigType_kDigital);
static_assert(
    static_cast<int>(CompressorConfigType::Analog) ==
    HAL_REVPHCompressorConfigType::HAL_REVPHCompressorConfigType_kAnalog);
static_assert(
    static_cast<int>(CompressorConfigType::Hybrid) ==
    HAL_REVPHCompressorConfigType::HAL_REVPHCompressorConfigType_kHybrid);

std::shared_ptr<PneumaticsBase> PneumaticsBase::GetForType(
    int busId, int module, PneumaticsModuleType moduleType) {
  if (moduleType == PneumaticsModuleType::CTREPCM) {
    return PneumaticsControlModule::GetForModule(busId, module);
  } else if (moduleType == PneumaticsModuleType::REVPH) {
    return PneumaticHub::GetForModule(busId, module);
  }
  throw WPILIB_MakeError(err::InvalidParameter, "{}",
                         static_cast<int>(moduleType));
}

int PneumaticsBase::GetDefaultForType(PneumaticsModuleType moduleType) {
  if (moduleType == PneumaticsModuleType::CTREPCM) {
    return SensorUtil::GetDefaultCTREPCMModule();
  } else if (moduleType == PneumaticsModuleType::REVPH) {
    return SensorUtil::GetDefaultREVPHModule();
  }
  throw WPILIB_MakeError(err::InvalidParameter, "{}",
                         static_cast<int>(moduleType));
}
