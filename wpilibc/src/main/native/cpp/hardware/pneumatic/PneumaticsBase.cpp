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
    static_cast<int>(CompressorConfigType::DISABLED) ==
    HAL_REVPHCompressorConfigType::HAL_REVPH_COMPRESSOR_CONFIG_DISABLED);
static_assert(
    static_cast<int>(CompressorConfigType::DIGITAL) ==
    HAL_REVPHCompressorConfigType::HAL_REVPH_COMPRESSOR_CONFIG_DIGITAL);
static_assert(
    static_cast<int>(CompressorConfigType::ANALOG) ==
    HAL_REVPHCompressorConfigType::HAL_REVPH_COMPRESSOR_CONFIG_ANALOG);
static_assert(
    static_cast<int>(CompressorConfigType::HYBRID) ==
    HAL_REVPHCompressorConfigType::HAL_REVPH_COMPRESSOR_CONFIG_HYBRID);

std::shared_ptr<PneumaticsBase> PneumaticsBase::GetForType(
    int busId, int module, PneumaticsModuleType moduleType) {
  if (moduleType == PneumaticsModuleType::CTRE_PCM) {
    return PneumaticsControlModule::GetForModule(busId, module);
  } else if (moduleType == PneumaticsModuleType::REV_PH) {
    return PneumaticHub::GetForModule(busId, module);
  }
  throw WPILIB_MakeError(err::InvalidParameter, "{}",
                         static_cast<int>(moduleType));
}

int PneumaticsBase::GetDefaultForType(PneumaticsModuleType moduleType) {
  if (moduleType == PneumaticsModuleType::CTRE_PCM) {
    return SensorUtil::GetDefaultCTREPCMModule();
  } else if (moduleType == PneumaticsModuleType::REV_PH) {
    return SensorUtil::GetDefaultREVPHModule();
  }
  throw WPILIB_MakeError(err::InvalidParameter, "{}",
                         static_cast<int>(moduleType));
}
